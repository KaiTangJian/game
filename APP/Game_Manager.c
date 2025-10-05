#include "Game_Manager.h"
#include "queue.h"
extern QueueHandle_t ui_request_queue;
GameState_t current_game_state;     // 当前游戏状态
const Level_t *current_level_data;  // 当前关卡数据
GamePlayer_t current_player1_state; // 冰人的当前状态
GamePlayer_t current_player2_state; // 火人的当前状态
uint32_t current_game_score;        // 当前游戏分数
uint32_t remaining_game_time_sec;   // 剩余游戏时间
extern bool OneNET_Upload_Game_Score(uint32_t score, uint8_t level);
#define MAX_GEMS_PER_LEVEL 8
// 消息类型定义

typedef enum {
    MSG_USER_ACTIVITY = 1,
    MSG_SCREEN_OFF,
    MSG_SCREEN_ON,
    MSG_GAME_STATE_CHANGE,
    MSG_VOLUME_CHANGE,
    MSG_WAKEUP,
    MSG_TIME_UPDATE
} AppMsgType_t;

typedef struct {
    AppMsgType_t type;
    uint32_t data;
} AppMessage_t;
typedef struct
{
    uint8_t x;
    uint8_t y;
    TileType_t type;
    bool collected;
} GemInfo_t;

static GemInfo_t gems[MAX_GEMS_PER_LEVEL];
static uint8_t gem_count = 0;

static bool is_move_valid(const GamePlayer_t *player, int8_t dx, int8_t dy)
{
    // 计算玩家尝试移动到的新瓦格
    int new_x = player->pos.x + dx;
    int new_y = player->pos.y + dy;

    // 1. 边界检�?: 确保新坐标在地图范围
    if (new_x < 0 || new_x >= MAP_WIDTH || new_y < 0 || new_y >= MAP_HEIGHT)
    {
        return false; // 越界，移动无
    }

     // 2. 对角移动特殊处理：防止穿越墙角
    if (dx != 0 && dy != 0) {
        // 检查两个相邻的墙角格子
        TileType_t horizontal_tile = (TileType_t)current_level_data->map_data[(uint16_t)player->pos.y][new_x];
        TileType_t vertical_tile = (TileType_t)current_level_data->map_data[new_y][(uint16_t)player->pos.x];
        
        // 如果任何一个相邻格子是墙壁，则不允许对角移动
        if (horizontal_tile == TILE_TYPE_WALL || vertical_tile == TILE_TYPE_WALL) {
            return false;
        }
    }
    
    // 使用当前关卡的地图数据进行查
    // 注意: current_level_data 必须在调用此函数前已有效加载
    TileType_t target_tile = (TileType_t)current_level_data->map_data[new_y][new_x];

    // 3. 墙壁检�?: 玩家不能穿过墙壁
    if (target_tile == TILE_TYPE_WALL)
    {
        return false; // 是墙壁，移动无效
    }
    // 5. 其他更复杂的碰撞检 (根据游戏设计添加)

    return true; // 所有检查通过，移动有
}
void Game_HandleInput(uint8_t player_id, int8_t dx, int8_t dy)
{
    GamePlayer_t *player = NULL;

    if (player_id == 1)
    {
        player = &current_player1_state;
    }
    else if (player_id == 2)
    {
        player = &current_player2_state;
    }
    else
    {
        return;
    }

    // 跳跃输入处理
    if (dy == -1 && player->on_ground && !player->is_jumping)
    {
        my_printf(&huart1, "JUMP");
        player->vertical_velocity = -5.5f; // 跳跃初速度（负值向上）
        player->is_jumping = true;
        player->on_ground = false;
        return;
    }
    if (is_move_valid(player, dx, dy))
    {
        player->pos.x += dx;
        player->pos.y += dy;
    }
}
bool Game_LoadLevel(uint8_t level_id)
{
    // 尝试加载关卡
    current_level_data = Level_GetById(level_id);
    if (current_level_data == NULL)
    {
        current_level_data = Level_GetById(1); // 默认关卡
        if (current_level_data == NULL)
        {
            return false; // 严重错误
        }
    }

    // 初始化冰人玩家（使用Point_t结构体）
    current_player1_state.type = PLAYER_TYPE_ICE;
    current_player1_state.pos = current_level_data->player1_start;
    current_player1_state.health = 100;
    current_player1_state.score = 0;
    current_player1_state.lives = 3;
    current_player1_state.is_alive = true;
    current_player1_state.is_invincible = false;
    current_player1_state.vertical_velocity = 0.0f;
    current_player1_state.on_ground = false;
    current_player1_state.is_jumping = false;

    // 初始化火人玩家
    current_player2_state.type = PLAYER_TYPE_FIRE;
    current_player2_state.pos = current_level_data->player2_start;
    current_player2_state.health = 100;
    current_player2_state.score = 0;
    current_player2_state.lives = 3;
    current_player2_state.is_alive = true;
    current_player2_state.is_invincible = false;
    current_player2_state.vertical_velocity = 0.0f;
    current_player2_state.on_ground = false;
    current_player2_state.is_jumping = false;

    // 宝石相关
    gem_count = 0;
    for (uint8_t y = 0; y < MAP_HEIGHT; ++y)
    {
        for (uint8_t x = 0; x < MAP_WIDTH; ++x)
        {
            TileType_t t = (TileType_t)current_level_data->map_data[y][x];
            if ((t == TILE_TYPE_COLLECTIBLE_FIRE_GEM || t == TILE_TYPE_COLLECTIBLE_ICE_GEM) && gem_count < MAX_GEMS_PER_LEVEL)
            {
                gems[gem_count].x = x;
                gems[gem_count].y = y;
                gems[gem_count].type = t;
                gems[gem_count].collected = false;
                gem_count++;
            }
        }
    }

    // 重置全局游戏状态
    current_game_score = 0;
    remaining_game_time_sec = current_level_data->time_limit_sec;
    current_game_state = GAME_STATE_PLAYING;

    return true; // 成功加载
}
void Game_Update(void)
{
        if (current_game_state == GAME_STATE_PLAYING && remaining_game_time_sec > 0) {
        static uint32_t last_time_update = 0;
        uint32_t current_time = xTaskGetTickCount(); // FreeRTOS系统时钟
        
        // 每1000ms减少1秒时间
        if (current_time - last_time_update >= configTICK_RATE_HZ) {
            remaining_game_time_sec--;
            last_time_update = current_time;
            
            // 检查时间是否用尽
            if (remaining_game_time_sec <= 0) 
            {
                remaining_game_time_sec = 0;
                Current_State = UI_STATE_LOSE; // 时间用尽，游戏结束
            }
        }
    } 
    // 1. 检查玩家与危险瓦格的碰撞
    TileType_t p1_current_tile = (TileType_t)current_level_data->map_data[(int)current_player1_state.pos.y][(int)current_player1_state.pos.x];
    if (current_player1_state.type == PLAYER_TYPE_ICE)
    {

        if (p1_current_tile == TILE_TYPE_FIRE)
        {
            current_player1_state.health--;
        }
    }

    TileType_t p2_current_tile = (TileType_t)current_level_data->map_data[(int)current_player2_state.pos.y][(int)current_player2_state.pos.x];
    if (current_player2_state.type == PLAYER_TYPE_FIRE)
    {
        // TODO: 定义 TILE_TYPE_WATER
        if (p2_current_tile == TILE_TYPE_ICE)
        {
            current_player2_state.health--;
        }
    }

    for (int i = 0; i < 2; ++i)
    {
        GamePlayer_t *player = (i == 0) ? &current_player1_state : &current_player2_state;
        
        int below_y = (int)(player->pos.y + 1);
        if (player->on_ground)
        {
            if (below_y >= MAP_HEIGHT || current_level_data->map_data[below_y][(int)player->pos.x] != TILE_TYPE_WALL)
            {
                // 脚下不是墙体，掉下去
                player->on_ground = false;
            }
        }
        if (!player->on_ground)
        {
            player->vertical_velocity += 1.0f;                              // 重力加速度
            float new_y = player->pos.y + player->vertical_velocity * 0.1f; // 0.1f为时间步长

            // 检查是否落地
            int tile_below = (int)(new_y + 1.0f);
            if (tile_below >= MAP_HEIGHT || current_level_data->map_data[tile_below][(int)player->pos.x] == TILE_TYPE_WALL)
            {
                player->pos.y = (float)((int)new_y); // 落地对齐格子
                player->vertical_velocity = 0.0f;
                player->on_ground = true;
                player->is_jumping = false;
            }
            else
            {
                player->pos.y = new_y;
            }
        }
		
		if (!player->on_ground && player->vertical_velocity < 0) // 玩家正在向上移动
	    {
            // 计算玩家头部位置（玩家当前位置减去一点偏移量）
			float head_pos = player->pos.y - 0.1f;
			int head_y = (int)head_pos; // 玩家头部所在的网格行
    
            // 检查是否撞到天花板（超出上边界或碰到墙体）
			if (head_y < 0 || current_level_data->map_data[head_y][(int)player->pos.x] == TILE_TYPE_WALL)
			{
                // 撞到天花板，停止向上移动
                player->vertical_velocity = 0.0f;
                 // 调整玩家位置，使其紧贴天花板下方
                player->pos.y = (float)(head_y + 1);
			}
	    }
    }

    // 冰人
    uint8_t p1x = (uint8_t)current_player1_state.pos.x;
    uint8_t p1y = (uint8_t)current_player1_state.pos.y;
    TileType_t p1_tile = (TileType_t)current_level_data->map_data[p1y][p1x];
    if ((p1_tile == TILE_TYPE_COLLECTIBLE_ICE_GEM) && !is_gem_collected(p1x, p1y))
    {
        // 标记为已收集
        for (uint8_t i = 0; i < gem_count; ++i)
        {
            if (gems[i].x == p1x && gems[i].y == p1y && !gems[i].collected)
            {
                gems[i].collected = true;
                current_game_score += 100;
                game_screen_redraw_tile(p1x, p1y); // 只重绘该格子
                //play_collect_item_sound();
                break;
            }
        }
    }
    // 火人
    uint8_t p2x = (uint8_t)current_player2_state.pos.x;
    uint8_t p2y = (uint8_t)current_player2_state.pos.y;
    TileType_t p2_tile = (TileType_t)current_level_data->map_data[p2y][p2x];
    if ((p2_tile == TILE_TYPE_COLLECTIBLE_FIRE_GEM) && !is_gem_collected(p2x, p2y))
    {
        for (uint8_t i = 0; i < gem_count; ++i)
        {
            if (gems[i].x == p2x && gems[i].y == p2y && !gems[i].collected)
            {
                gems[i].collected = true;
                current_game_score += 100;
                game_screen_redraw_tile(p2x, p2y); // 只重绘该格子
                play_collect_item_sound();
                break;
            }
        }
    }

    // 2. 检查游戏胜利条件
    if (p1_current_tile == TILE_TYPE_EXIT && p2_current_tile == TILE_TYPE_EXIT)
    {
        // 游戏胜利时将分数添加到排行榜
        extern void add_score_to_leaderboard(uint8_t level_id, uint32_t score);
        add_score_to_leaderboard(current_level_data->id, current_game_score);
        // 上传分数到OneNET平台
        
        OneNET_Upload_Game_Score(current_game_score, current_level_data->id);
                // 发送胜利切换请求
        AppMessage_t req_msg = {MSG_GAME_STATE_CHANGE, UI_STATE_WON};
        xQueueSend(ui_request_queue, &req_msg, 0);
    }

    // 4. 检查游戏失败条件 (除了时间用尽)
    if (current_player1_state.health <= 0 || current_player2_state.health <= 0 || remaining_game_time_sec <= 0)
    {
        AppMessage_t req_msg = {MSG_GAME_STATE_CHANGE, UI_STATE_LOSE};
        xQueueSend(ui_request_queue, &req_msg, 0);
    }
}

bool is_gem_collected(uint8_t x, uint8_t y)
{
    for (uint8_t i = 0; i < gem_count; ++i)
    {
        if (gems[i].x == x && gems[i].y == y)
            return gems[i].collected;
    }
    return false;
}
