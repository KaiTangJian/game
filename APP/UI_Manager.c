#include "UI_Manager.h"
#include "lvgl.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
// 密码相关变量
#define MAX_PASSWORD_LENGTH 8
#define DEFAULT_PASSWORD "1221"
char system_password[MAX_PASSWORD_LENGTH + 1] = DEFAULT_PASSWORD;
char password_input_buffer[MAX_PASSWORD_LENGTH + 1] = {0};
uint8_t password_input_index = 0;
extern QueueHandle_t lvgl_mutex;
extern QueueHandle_t ui_request_queue;
// 密码界面对象
lv_obj_t *password_screen = NULL;
lv_obj_t *password_title_label = NULL;
lv_obj_t *password_input_label = NULL;
lv_obj_t *password_hint_label = NULL;

UI_STATE_t Current_State = UI_STATE_START; // 现在的ui界面
int Select_Number = 1;                     // 当前选择的关卡
lv_obj_t *Home_Screen;
lv_obj_t *Select_Screen;
lv_obj_t *Select_Label;
extern bool is_gem_collected(uint8_t x, uint8_t y);
static lv_color_t canvas_buffer[LV_CANVAS_BUF_SIZE_TRUE_COLOR(MAP_WIDTH * TILE_SIZE, MAP_HEIGHT *TILE_SIZE)];
typedef enum
{
    MSG_USER_ACTIVITY = 1,
    MSG_SCREEN_OFF,
    MSG_SCREEN_ON,
    MSG_GAME_STATE_CHANGE,
    MSG_VOLUME_CHANGE,
    MSG_WAKEUP,
    MSG_TIME_UPDATE
} AppMsgType_t;

typedef struct
{
    AppMsgType_t type;
    uint32_t data;
} AppMessage_t;

// 5个关卡的排行榜数据
uint32_t level_high_scores[TOTAL_LEVELS][MAX_SCORES_PER_LEVEL] = {
    {0, 0, 0, 0, 0}, // Level 1 的前5名分数
    {0, 0, 0, 0, 0}, // Level 2 的前5名分数
    {0, 0, 0, 0, 0}, // Level 3 的前5名分数
    {0, 0, 0, 0, 0}, // Level 4 的前5名分数
    {0, 0, 0, 0, 0}  // Level 5 的前5名分数
};
static lv_obj_t *score_labels[TOTAL_LEVELS][MAX_SCORES_PER_LEVEL] = {{NULL}};
static lv_obj_t *level_title_labels[TOTAL_LEVELS] = {NULL};

void add_score_to_leaderboard(uint8_t level_id, uint32_t score)
{
    // 检查关卡ID有效性
    if (level_id < 1 || level_id > 5)
        return;

    uint8_t index = level_id - 1; // 转换为0基索引

    // 找到合适的插入位置
    for (int i = 0; i < MAX_SCORES_PER_LEVEL; i++)
    {
        if (score > level_high_scores[index][i])
        {
            // 将较低的分数下移
            for (int j = MAX_SCORES_PER_LEVEL - 1; j > i; j--)
            {
                level_high_scores[index][j] = level_high_scores[index][j - 1];
            }
            // 插入新分数
            level_high_scores[index][i] = score;
            break;
        }
    }
    // 保存到Flash
    extern bool Flash_WriteScores(void);
    Flash_WriteScores();
    // 更新UI显示（如果主界面已创建）
    if (Home_Screen != NULL)
    {
        // 只更新当前关卡的前三名显示
        for (int i = 0; i < 3; i++)
        {
            if (score_labels[index][i] != NULL)
            {
                char score_text[16];
                snprintf(score_text, sizeof(score_text), "%d.%d", i + 1, (int)level_high_scores[index][i]);
                lv_label_set_text(score_labels[index][i], score_text);
            }
        }
    }
}

/**
 * @brief 更新主界面的排行榜显示
 */
void update_home_screen_leaderboard(void)
{
    if (Home_Screen == NULL)
        return;

    // 直接在主界面上创建排行榜元素
    static bool leaderboard_created = false;

    if (!leaderboard_created)
    {

        // 横排显示5个关卡的前三名
        int start_x = 20;
        int start_y = 180;
        int level_spacing = 60; // 每个关卡之间的水平间距

        // 为每个关卡创建排行榜显示区域
        for (int level = 0; level < 5; level++)
        {
            // 关卡标题
            level_title_labels[level] = lv_label_create(Home_Screen);
            char title_text[8];
            snprintf(title_text, sizeof(title_text), "L%d", level + 1);
            lv_label_set_text(level_title_labels[level], title_text);
            lv_obj_set_style_text_color(level_title_labels[level], lv_color_hex(0x0000FF), 0);
            lv_obj_align(level_title_labels[level], LV_ALIGN_TOP_LEFT, start_x + level * level_spacing, start_y);

            // 分数条目 (显示前3名)
            for (int i = 0; i < 3; i++)
            {
                score_labels[level][i] = lv_label_create(Home_Screen);
                lv_obj_set_style_text_color(score_labels[level][i], lv_color_hex(0x000000), 0);
                char score_text[16];
                snprintf(score_text, sizeof(score_text), "%d.%d", i + 1, (int)level_high_scores[level][i]);
                lv_label_set_text(score_labels[level][i], score_text);
                lv_obj_align(score_labels[level][i], LV_ALIGN_TOP_LEFT,
                             start_x + level * level_spacing, start_y + 15 + i * 15);
            }
        }

        leaderboard_created = true;
    }
    else
    {
        // 更新现有标签的文本
        for (int level = 0; level < 5; level++)
        {
            for (int i = 0; i < 3; i++)
            {
                char score_text[16];
                snprintf(score_text, sizeof(score_text), "%d.%d", i + 1, (int)level_high_scores[level][i]);
                lv_label_set_text(score_labels[level][i], score_text);
            }
        }
    }
}

void create_home_screen(void)
{
    if (Home_Screen == NULL)
    {
        Home_Screen = lv_obj_create(NULL); // 创建一个新屏幕
        lv_obj_set_style_bg_color(Home_Screen, lv_color_hex(0xFFFF), 0);

        lv_obj_t *title = lv_label_create(Home_Screen);
        lv_label_set_text(title, "FIRE AND ICE");
        lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 50);
        // lv_obj_set_style_text_color(title, lv_color_hex(0xFF0000), 0); // 红色文字
        lv_obj_set_style_text_color(title, lv_color_hex(0xF800), 0);
        lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0); // 24号字体，可根据需要调??
        lv_obj_t *hint_start = lv_label_create(Home_Screen);
        lv_label_set_text(hint_start, "Buttom1:Enter");
        lv_obj_align(hint_start, LV_ALIGN_TOP_LEFT, 10, 10);

        lv_obj_t *hint_exit = lv_label_create(Home_Screen);
        lv_label_set_text(hint_exit, "Buttom2:Exit");
        lv_obj_align_to(hint_exit, hint_start, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
        update_home_screen_leaderboard();
    }
}

lv_obj_t *game_play_screen = NULL;
static lv_obj_t *map_canvas = NULL; // ??????????????

// ?????? (????????????)
static lv_obj_t *player1_obj = NULL;
static lv_obj_t *player2_obj = NULL;

// UI ???
static lv_obj_t *score_label = NULL;
static lv_obj_t *p1_health_label = NULL;
static lv_obj_t *p2_health_label = NULL;
static lv_obj_t *time_label = NULL;

lv_obj_t *game_win_screen = NULL;
lv_obj_t *game_lose_screen = NULL;

// =============================================================================
// ???????
// =============================================================================

/**
 * @brief 游戏界面绘制
 */
void create_game_play_screen(void)
{
    if (game_play_screen == NULL)
    {
        game_play_screen = lv_obj_create(NULL);
        lv_obj_set_style_bg_color(game_play_screen, lv_color_hex(0xFFFFFF), LV_PART_MAIN); // ???????
        lv_obj_clear_flag(game_play_screen, LV_OBJ_FLAG_SCROLLABLE);                       // ???ù???

        // ???????????
        map_canvas = lv_canvas_create(game_play_screen);
        lv_canvas_set_buffer(map_canvas, canvas_buffer, MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE, LV_IMG_CF_TRUE_COLOR);
        lv_obj_align(map_canvas, LV_ALIGN_CENTER, 0, 0); // ????
        lv_obj_clear_flag(map_canvas, LV_OBJ_FLAG_SCROLLABLE);

        // 冰人
        player1_obj = lv_obj_create(game_play_screen);
        lv_obj_set_size(player1_obj, TILE_SIZE, TILE_SIZE);
        lv_obj_set_style_bg_color(player1_obj, lv_color_hex(0xFF4500), LV_PART_MAIN); // ?????
        lv_obj_set_style_border_width(player1_obj, 0, LV_PART_MAIN);                  // ????
        lv_obj_clear_flag(player1_obj, LV_OBJ_FLAG_SCROLLABLE);
        // lv_obj_add_flag(player1_obj, LV_OBJ_FLAG_HIDDEN); // ???????

        // ?火人
        player2_obj = lv_obj_create(game_play_screen);
        lv_obj_set_size(player2_obj, TILE_SIZE, TILE_SIZE);
        lv_obj_set_style_bg_color(player2_obj, lv_color_hex(0xF800), LV_PART_MAIN); // ????
        lv_obj_set_style_border_width(player2_obj, 0, LV_PART_MAIN);                // ????
        lv_obj_clear_flag(player2_obj, LV_OBJ_FLAG_SCROLLABLE);
        // lv_obj_add_flag(player2_obj, LV_OBJ_FLAG_HIDDEN); // ???????

        // ???? UI ????? (?÷????????????)
        score_label = lv_label_create(game_play_screen);
        lv_obj_set_style_text_color(score_label, lv_color_hex(0xFF4500), LV_PART_MAIN);
        lv_obj_align(score_label, LV_ALIGN_TOP_LEFT, 10, 10);
        lv_label_set_text(score_label, "Score: 0");

        p1_health_label = lv_label_create(game_play_screen);
        lv_obj_set_style_text_color(p1_health_label, lv_color_hex(0x00BFFF), LV_PART_MAIN);
        lv_obj_align_to(p1_health_label, score_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
        lv_label_set_text(p1_health_label, "Ice Health: 3");

        p2_health_label = lv_label_create(game_play_screen);
        lv_obj_set_style_text_color(p2_health_label, lv_color_hex(0xFF4500), LV_PART_MAIN);
        lv_obj_align_to(p2_health_label, p1_health_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
        lv_label_set_text(p2_health_label, "Fire Health: 3");

        time_label = lv_label_create(game_play_screen);
        lv_obj_set_style_text_color(time_label, lv_color_hex(0xFF4500), LV_PART_MAIN);
        lv_obj_align(time_label, LV_ALIGN_TOP_RIGHT, -10, 10);
        lv_label_set_text(time_label, "Time: 00:00");
    }
}

/**
 * @brief 地图绘制
 * @param level_data 地图数据
 */
void game_screen_draw_map(const Level_t *level_data)
{
    if (!map_canvas || !level_data)
        return;

    // 清空画布
    lv_canvas_fill_bg(map_canvas, lv_color_hex(0x333333), LV_OPA_COVER);

    // 声明一个矩形绘图描述符
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.border_width = 0;
    rect_dsc.radius = 0;

    // 声明圆形绘图描述符
    lv_draw_arc_dsc_t arc_dsc;
    lv_draw_arc_dsc_init(&arc_dsc);
    arc_dsc.width = TILE_SIZE / 3;

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            TileType_t tile = (TileType_t)level_data->map_data[y][x];

            // 先绘制基础地形背景
            switch (tile)
            {
            case TILE_TYPE_WALL:
                rect_dsc.bg_color = lv_color_hex(0x888888); // 灰色
                break;
            case TILE_TYPE_FIRE:
                rect_dsc.bg_color = lv_color_hex(0xF800); // 红色
                break;
            case TILE_TYPE_ICE:
                rect_dsc.bg_color = lv_color_hex(0xAA0000); // 浅蓝色
                break;
            case TILE_TYPE_EXIT:
                rect_dsc.bg_color = lv_color_hex(0x07E0); // 绿色
                break;
            case TILE_TYPE_COLLECTIBLE_FIRE_GEM:
            case TILE_TYPE_COLLECTIBLE_ICE_GEM:
                rect_dsc.bg_color = lv_color_hex(0xFFE0); // 宝石格子使用普通地面颜色
                break;
            default:
                rect_dsc.bg_color = lv_color_hex(0x000000);
                break;
            }

            // 绘制基础格子
            lv_canvas_draw_rect(map_canvas, x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, &rect_dsc);

            // 如果是宝石，则在上方绘制圆形
            switch (tile)
            {
            case TILE_TYPE_COLLECTIBLE_FIRE_GEM:
                arc_dsc.color = lv_color_make(255, 165, 0); // 橙色
                lv_canvas_draw_arc(map_canvas, x * TILE_SIZE + TILE_SIZE / 2, y * TILE_SIZE + TILE_SIZE / 2,
                                   TILE_SIZE / 3, 0, 360, &arc_dsc);
                break;
            case TILE_TYPE_COLLECTIBLE_ICE_GEM:
                arc_dsc.color = lv_color_make(173, 216, 230); // 浅蓝色
                lv_canvas_draw_arc(map_canvas, x * TILE_SIZE + TILE_SIZE / 2, y * TILE_SIZE + TILE_SIZE / 2,
                                   TILE_SIZE / 3, 0, 360, &arc_dsc);
                break;
            default:
                break;
            }
        }
    }
    lv_obj_invalidate(map_canvas);
}

/**
 * @brief 动态元素更�?
 * @param p1 ????????????
 * @param p2 ????????????
 */
void game_screen_update_dynamic_elements(const GamePlayer_t *p1, const GamePlayer_t *p2)
{

    if (player1_obj)
    {
        // 获取地图画布的屏幕坐�?
        lv_coord_t canvas_x = lv_obj_get_x(map_canvas);
        lv_coord_t canvas_y = lv_obj_get_y(map_canvas);

        // 计算玩家相对于地图画布的坐标
        lv_coord_t player_x = (int)p1->pos.x * TILE_SIZE;
        lv_coord_t player_y = (int)p1->pos.y * TILE_SIZE;

        // 设置玩家在屏幕上的绝对位�?
        lv_obj_set_pos(player1_obj, canvas_x + player_x, canvas_y + player_y);
        lv_obj_clear_flag(player1_obj, LV_OBJ_FLAG_HIDDEN); // 显示冰人
    }

    // 火人位置更新
    if (player2_obj)
    {
        // 获取地图画布的屏幕坐�?
        lv_coord_t canvas_x = lv_obj_get_x(map_canvas);
        lv_coord_t canvas_y = lv_obj_get_y(map_canvas);

        // 计算玩家相对于地图画布的坐标
        lv_coord_t player_x = (int)p2->pos.x * TILE_SIZE;
        lv_coord_t player_y = (int)p2->pos.y * TILE_SIZE;

        // 设置玩家在屏幕上的绝对位�?
        lv_obj_set_pos(player2_obj, canvas_x + player_x, canvas_y + player_y);
        lv_obj_clear_flag(player2_obj, LV_OBJ_FLAG_HIDDEN); // 显示火人
    }
}

/**
 * @brief UI界面更新
 * @param score ???????÷??
 * @param p1_health ???????????
 * @param p2_health ???????????
 * @param time_sec ?????????????
 */
void game_screen_update_ui_overlay(uint32_t score, uint32_t p1_health, uint32_t p2_health, uint32_t time_sec)
{
    lv_label_set_text_fmt(score_label, "Score: %d", score);
    lv_label_set_text_fmt(p1_health_label, "Ice Health: %d", p1_health);
    lv_label_set_text_fmt(p2_health_label, "Fire Health: %d", p2_health);

    uint32_t minutes = time_sec / 60;
    uint32_t seconds = time_sec % 60;
    lv_label_set_text_fmt(time_label, "Time: %02d:%02d", minutes, seconds);
}

/**
 * @brief 胜利界面
 */
void create_game_win_screen(void)
{
    if (game_win_screen == NULL)
    {
        game_win_screen = lv_obj_create(NULL);
        lv_obj_set_style_bg_color(game_win_screen, lv_color_hex(0x00AA00), LV_PART_MAIN); // ???????
        lv_obj_set_style_text_color(game_win_screen, lv_color_white(), LV_PART_MAIN);
        lv_obj_clear_flag(game_win_screen, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *label = lv_label_create(game_win_screen);
        lv_label_set_text(label, "YOU WIN!\nCongratulations!");
        lv_obj_center(label);
    }
}

/**
 * @brief 失败界面
 */
void create_game_lose_screen(void)
{
    if (game_lose_screen == NULL)
    {
        game_lose_screen = lv_obj_create(NULL);
        lv_obj_set_style_bg_color(game_lose_screen, lv_color_hex(0xAA0000), LV_PART_MAIN); // ???????
        lv_obj_set_style_text_color(game_lose_screen, lv_color_white(), LV_PART_MAIN);
        lv_obj_clear_flag(game_lose_screen, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *label = lv_label_create(game_lose_screen);
        lv_label_set_text(label, "GAME OVER!\nTry again!");
        lv_obj_center(label);
    }
}

// ...existing code...
lv_obj_t *level_labels[5] = {NULL}; // 全局声明

void create_select_screen()
{
    if (Select_Screen == NULL)
    {
        Select_Screen = lv_obj_create(NULL);
        lv_obj_set_style_bg_color(Select_Screen, lv_color_hex(0xAA0000), LV_PART_MAIN);
        lv_obj_set_style_text_color(Select_Screen, lv_color_white(), LV_PART_MAIN);
        lv_obj_clear_flag(Select_Screen, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *title = lv_label_create(Select_Screen);
        lv_label_set_text(title, "SELECT LEVEL");
        lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 30);

        for (int i = 0; i < 5; ++i)
        {
            level_labels[i] = lv_label_create(Select_Screen);
            char buf[16];
            snprintf(buf, sizeof(buf), "Level %d", i + 1);
            lv_label_set_text(level_labels[i], buf);

            if (Select_Number == i + 1)
            {
                lv_obj_set_style_text_color(level_labels[i], lv_color_hex(0xFFFF00), LV_PART_MAIN);
                // lv_obj_set_style_text_font(level_labels[i], &lv_font_montserrat_24, LV_PART_MAIN);
            }
            else
            {
                lv_obj_set_style_text_color(level_labels[i], lv_color_white(), LV_PART_MAIN);
                // lv_obj_set_style_text_font(level_labels[i], &lv_font_montserrat_18, LV_PART_MAIN);
            }
            lv_obj_align(level_labels[i], LV_ALIGN_TOP_MID, 0, 80 + i * 40);
        }

        lv_obj_t *hint = lv_label_create(Select_Screen);
        lv_label_set_text(hint, "Use Up/Down to select, Enter to confirm");
        lv_obj_align(hint, LV_ALIGN_BOTTOM_MID, 0, -20);
    }
}

// 新增：只更新label高亮
void update_level_labels_highlight(void)
{
    for (int i = 0; i < 5; ++i)
    {
        if (level_labels[i])
        {
            if (Select_Number == i + 1)
            {
                lv_obj_set_style_text_color(level_labels[i], lv_color_hex(0xFFFF00), LV_PART_MAIN);
            }
            else
            {
                lv_obj_set_style_text_color(level_labels[i], lv_color_white(), LV_PART_MAIN);
            }
        }
    }
}

void game_screen_redraw_tile(uint8_t x, uint8_t y)
{
    TileType_t t = (TileType_t)current_level_data->map_data[y][x];
    if ((t == TILE_TYPE_COLLECTIBLE_FIRE_GEM || t == TILE_TYPE_COLLECTIBLE_ICE_GEM) && is_gem_collected(x, y))
    {
        t = TILE_TYPE_NORMAL;
    }

    // 初始化矩形绘制描述符
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.border_width = 0;
    rect_dsc.radius = 0;

    // 先绘制基础地形背景
    switch (t)
    {
    case TILE_TYPE_WALL:
        rect_dsc.bg_color = lv_color_hex(0x888888); // 灰色
        break;
    case TILE_TYPE_FIRE:
        rect_dsc.bg_color = lv_color_hex(0xF800); // 红色
        break;
    case TILE_TYPE_ICE:
        rect_dsc.bg_color = lv_color_hex(0x001F); // 浅蓝色
        break;
    case TILE_TYPE_EXIT:
        rect_dsc.bg_color = lv_color_hex(0x07E0); // 绿色
        break;
    case TILE_TYPE_COLLECTIBLE_FIRE_GEM:
    case TILE_TYPE_COLLECTIBLE_ICE_GEM:
        rect_dsc.bg_color = lv_color_hex(0xFFE0); // 宝石格子使用普通地面颜色作为背景
        break;
    default:
        rect_dsc.bg_color = lv_color_hex(0x000000); // 普通地面
        break;
    }

    // 绘制基础格子背景
    lv_canvas_draw_rect(map_canvas, x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, &rect_dsc);

    // 如果是宝石，则在上方绘制圆形
    if (t == TILE_TYPE_COLLECTIBLE_FIRE_GEM || t == TILE_TYPE_COLLECTIBLE_ICE_GEM)
    {
        lv_draw_arc_dsc_t arc_dsc;
        lv_draw_arc_dsc_init(&arc_dsc);
        arc_dsc.width = TILE_SIZE / 3;

        if (t == TILE_TYPE_COLLECTIBLE_FIRE_GEM)
        {
            arc_dsc.color = lv_color_make(255, 165, 0); // 橙色宝石
            lv_canvas_draw_arc(map_canvas, x * TILE_SIZE + TILE_SIZE / 2, y * TILE_SIZE + TILE_SIZE / 2,
                               TILE_SIZE / 3, 0, 360, &arc_dsc);
        }
        else if (t == TILE_TYPE_COLLECTIBLE_ICE_GEM)
        {
            arc_dsc.color = lv_color_make(173, 216, 230); // 浅蓝色宝石
            lv_canvas_draw_arc(map_canvas, x * TILE_SIZE + TILE_SIZE / 2, y * TILE_SIZE + TILE_SIZE / 2,
                               TILE_SIZE / 3, 0, 360, &arc_dsc);
        }
    }

    lv_obj_invalidate(map_canvas); // 通知LVGL刷新
}

void create_password_screen(void)
{
    if (password_screen != NULL)
        return;

    password_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(password_screen, lv_color_hex(0xFFFF), 0);

    // 标题
    password_title_label = lv_label_create(password_screen);
    lv_label_set_text(password_title_label, "Please input your password");
    lv_obj_set_style_text_font(password_title_label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(password_title_label, lv_color_hex(0x000000), 0);
    lv_obj_align(password_title_label, LV_ALIGN_TOP_MID, 0, 30);

    // 密码输入显示区域
    password_input_label = lv_label_create(password_screen);
    lv_label_set_text(password_input_label, "");
    lv_obj_set_style_text_font(password_input_label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(password_input_label, lv_color_hex(0x0000FF), 0);
    lv_obj_align(password_input_label, LV_ALIGN_CENTER, 0, 0);

    // 提示信息
    password_hint_label = lv_label_create(password_screen);
    lv_label_set_text(password_hint_label, "Key1: Input 1\nKey2: Input 2\nKey3: Enter\nKey4:Delete");
    lv_obj_set_style_text_color(password_hint_label, lv_color_hex(0x888888), 0);
    lv_obj_align(password_hint_label, LV_ALIGN_BOTTOM_MID, 0, -30);
}

void password_input_digit(int digit)
{
    if (password_input_index < MAX_PASSWORD_LENGTH)
    {
        password_input_buffer[password_input_index] = digit + '0';
        password_input_index++;
        password_input_buffer[password_input_index] = '\0';

        // 更新显示为*
        char stars[MAX_PASSWORD_LENGTH + 1];
        for (int i = 0; i < password_input_index; i++)
        {
            stars[i] = '*';
        }
        stars[password_input_index] = '\0';

        if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            lv_label_set_text(password_input_label, stars);
            xSemaphoreGive(lvgl_mutex);
        }
    }
}

void password_backspace(void)
{
    if (password_input_index > 0)
    {
        password_input_index--;
        password_input_buffer[password_input_index] = '\0';

        // 更新显示
        char stars[MAX_PASSWORD_LENGTH + 1];
        for (int i = 0; i < password_input_index; i++)
        {
            stars[i] = '*';
        }
        stars[password_input_index] = '\0';

        if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            lv_label_set_text(password_input_label, stars);
            xSemaphoreGive(lvgl_mutex);
        }
    }
}

void password_confirm(void)
{
    if (strcmp(password_input_buffer, system_password) == 0)
    {
        // 密码正确
        my_printf(&huart1, "密码验证通过\r\n");
        password_input_index = 0;
        memset(password_input_buffer, 0, sizeof(password_input_buffer));

        // 切换到主界面
        AppMessage_t msg = {MSG_GAME_STATE_CHANGE, UI_STATE_START};
        xQueueSend(ui_request_queue, &msg, 0);
    }
    else
    {
        // 密码错误
        my_printf(&huart1, "密码错误\r\n");

        // 显示错误信息
        if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            lv_label_set_text(password_input_label, "密码错误!");
            xSemaphoreGive(lvgl_mutex);

            // 1秒后清空显示并重置输入
            vTaskDelay(pdMS_TO_TICKS(1000));
            password_input_index = 0;
            memset(password_input_buffer, 0, sizeof(password_input_buffer));
            if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE)
            {
                lv_label_set_text(password_input_label, "");
                xSemaphoreGive(lvgl_mutex);
            }
        }
    }
}
