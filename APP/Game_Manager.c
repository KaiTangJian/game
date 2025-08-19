#include "Game_Manager.h"
GameState_t current_game_state;     // 当前游戏状�?
const Level_t *current_level_data;  // 当前关卡数据
GamePlayer_t current_player1_state; // 冰人的当前状�?
GamePlayer_t current_player2_state; // 火人的当前状�?
uint32_t current_game_score;        // 当前游戏分数
uint32_t remaining_game_time_sec;   // 剩余游戏时间（秒�?

static bool is_move_valid(const GamePlayer_t *player, int8_t dx, int8_t dy)
{
    // 计算玩家尝试移动到的新瓦格坐�?
    int new_x = player->pos.x + dx;
    int new_y = player->pos.y + dy;

    // 1. 边界检�?: 确保新坐标在地图范围�?
    if (new_x < 0 || new_x >= MAP_WIDTH || new_y < 0 || new_y >= MAP_HEIGHT)
    {
        return false; // 越界，移动无�?
    }

    // 2. 获取目标瓦格的类�?
    // 使用当前关卡的地图数据进行查�?
    // 注意: current_level_data 必须在调用此函数前已有效加载�?
    TileType_t target_tile = (TileType_t)current_level_data->map_data[new_y][new_x];

    // 3. 墙壁检�?: 玩家不能穿过墙壁
    if (target_tile == TILE_TYPE_WALL)
    {
        return false; // 是墙壁，移动无效
    }

    // 4. 特定玩家类型与危险地形的交互检�?
    if (player->type == PLAYER_TYPE_ICE)
    { // 如果是冰�?
        // 冰人不能进入岩浆或火焰瓦�?
        // TODO: 定义 TILE_TYPE_LAVA，目前你�? TileType_t 中只�? FIRE/ICE/NORMAL/EXIT...
        // 假设 TILE_TYPE_LAVA 是存在的
        if (target_tile == TILE_TYPE_FIRE)
        {
            return false;
        }
    }
    else
    { // 如果是火�? (PLAYER_TYPE_FIRE)
        // 火人不能进入水域或冰川瓦�?
        // TODO: 定义 TILE_TYPE_WATER，目前你�? TileType_t 中只�? FIRE/ICE/NORMAL/EXIT...
        // 假设 TILE_TYPE_WATER 是存在的
        if (target_tile == TILE_TYPE_ICE)
        {
            return false;
        }
    }

    // 5. 其他更复杂的碰撞检�? (根据游戏设计添加)

    return true; // 所有检查通过，移动有�?
}
void Game_HandleInput(uint8_t player_id, int8_t dx, int8_t dy)
{
    GamePlayer_t *player = NULL;
    my_printf(&huart1, "Input: dx=%d dy=%d on_ground=%d is_jumping=%d\r\n", dx, dy, player->on_ground, player->is_jumping);
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
        player->vertical_velocity = -1.0f; // 跳跃初速度（负值向上）
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
    // 尝试加载关卡（带错误恢复�?
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
    // ...相同属性初始化...

    // 重置全局游戏状态
    current_game_score = 0;
    remaining_game_time_sec = current_level_data->time_limit_sec;
    current_game_state = GAME_STATE_PLAYING;

    return true; // 成功加载
}
void Game_Update(void)
{
    // 1. 检查玩家与危险瓦格的碰撞
    TileType_t p1_current_tile = (TileType_t)current_level_data->map_data[(int)current_player1_state.pos.y][(int)current_player1_state.pos.x];
    if (current_player1_state.type == PLAYER_TYPE_ICE)
    {
        // TODO: 定义 TILE_TYPE_LAVA
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
        if (!player->on_ground) {
            player->vertical_velocity += 0.3f; // 重力加速度
            float new_y = player->pos.y + player->vertical_velocity * 0.1f; // 0.1f为时间步长

            // 检查是否落地
            int tile_below = (int)(new_y + 1.0f);
            if (tile_below >= MAP_HEIGHT ||current_level_data->map_data[tile_below][(int)player->pos.x] == TILE_TYPE_WALL) 
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
        my_printf(&huart1, "Update: y=%.2f on_ground=%d is_jumping=%d v=%.2f\r\n", player->pos.y, player->on_ground, player->is_jumping, player->vertical_velocity);
    }
    

    // 2. 检查游戏胜利条件
    if (p1_current_tile == TILE_TYPE_EXIT && p2_current_tile == TILE_TYPE_EXIT)
    {
        current_game_state = GAME_STATE_WON;
    }

   

    // 4. 检查游戏失败条件 (除了时间用尽)
    if (current_player1_state.health <= 0 || current_player2_state.health <= 0)
    {
        current_game_state = GAME_STATE_LOST;
    }
}
