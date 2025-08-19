#include "UI_Manager.h"

UI_STATE_t Current_State = UI_STATE_START; // 现在的ui界面
int Select_Number = 1;                     // 当前选择的关卡
lv_obj_t *Home_Screen;
lv_obj_t *Select_Screen;
lv_obj_t *Select_Label;
static lv_color_t canvas_buffer[LV_CANVAS_BUF_SIZE_TRUE_COLOR(MAP_WIDTH * TILE_SIZE, MAP_HEIGHT *TILE_SIZE)];
void create_home_screen(void)
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
}

// =============================================================================
// LVGL ????????
// =============================================================================
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

        // ?????????? (???? - ?????????)
        player1_obj = lv_obj_create(game_play_screen);
        lv_obj_set_size(player1_obj, TILE_SIZE, TILE_SIZE);
        lv_obj_set_style_bg_color(player1_obj, lv_color_hex(0x0000), LV_PART_MAIN); // ?????
        lv_obj_set_style_border_width(player1_obj, 0, LV_PART_MAIN);                  // ????
        lv_obj_clear_flag(player1_obj, LV_OBJ_FLAG_SCROLLABLE);
        //lv_obj_add_flag(player1_obj, LV_OBJ_FLAG_HIDDEN); // ???????

        // ?????????? (???? - ????????)
        player2_obj = lv_obj_create(game_play_screen);
        lv_obj_set_size(player2_obj, TILE_SIZE, TILE_SIZE);
        lv_obj_set_style_bg_color(player2_obj, lv_color_hex(0xFF4500), LV_PART_MAIN); // ????
        lv_obj_set_style_border_width(player2_obj, 0, LV_PART_MAIN);                  // ????
        lv_obj_clear_flag(player2_obj, LV_OBJ_FLAG_SCROLLABLE);
       // lv_obj_add_flag(player2_obj, LV_OBJ_FLAG_HIDDEN); // ???????

        // ???? UI ????? (?÷????????????)
        score_label = lv_label_create(game_play_screen);
        lv_obj_set_style_text_color(score_label, lv_color_white(), LV_PART_MAIN);
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
 * @param level_data ???????????
 */
void game_screen_draw_map(const Level_t *level_data)
{
    if (!map_canvas || !level_data)
        return;

    // ?????????????????????
    lv_canvas_fill_bg(map_canvas, lv_color_hex(0x333333), LV_OPA_COVER); // ???????????

    // 声明一个矩形绘图描述符
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc); // 初始化绘图描述符

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            TileType_t tile = (TileType_t)level_data->map_data[y][x];
            //lv_color_t tile_color = lv_color_hex(0x333333); // ???????????? TILE_TYPE_NORMAL ????????
            lv_color_t tile_color = lv_color_make(51, 51, 51); // 手动指定RGB�?
            switch (tile)
            {
            case TILE_TYPE_WALL:
                tile_color = lv_color_hex(0X8430); // 灰色
                break;
            case TILE_TYPE_FIRE:
                tile_color = lv_color_hex(0xF800); // 红色
                break;
            case TILE_TYPE_ICE:
                tile_color = lv_color_hex(0x001F); // 浅蓝�?
                break;
            case TILE_TYPE_EXIT:
                tile_color = lv_color_hex(0x07E0); // 绿色
                break;
            // TILE_TYPE_NORMAL ??????δ????????????????????? 0x333333
            default:
                tile_color = lv_color_hex(0xFFE0);
                break;
            }
            // 设置绘图描述符的背景颜色
            rect_dsc.bg_color = tile_color;
            // 如果不需要边框，确保边框宽度�?0
            rect_dsc.border_width = 0;
            // 如果不需要圆角，确保圆角半径�?0
            rect_dsc.radius = 0;
            // ????????????????
            lv_canvas_draw_rect(map_canvas, x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, &rect_dsc);
        }
    }
    lv_obj_invalidate(map_canvas); // ?????????
}

/**
 * @brief 动态元素更�?
 * @param p1 ????????????
 * @param p2 ????????????
 */
void game_screen_update_dynamic_elements(const GamePlayer_t *p1, const GamePlayer_t *p2)
{
//    // ???±???λ??
//    if (player1_obj)
//    {
//        lv_obj_set_pos(player1_obj, (int)p1->pos.x * TILE_SIZE, (int)p1->pos.y * TILE_SIZE);
//        lv_obj_clear_flag(player1_obj, LV_OBJ_FLAG_HIDDEN); // ????????
//    }

//    // ???????λ??
//    if (player2_obj)
//    {
//        lv_obj_set_pos(player2_obj, (int)p2->pos.x * TILE_SIZE, (int)p2->pos.y * TILE_SIZE);
//        lv_obj_clear_flag(player2_obj, LV_OBJ_FLAG_HIDDEN); // ????????
//    }
//    // ??????????????????�????
	    // 冰人位置更新
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

void create_select_screen()
{
    if (Select_Screen == NULL)
    {
        Select_Screen = lv_obj_create(NULL);
        lv_obj_set_style_bg_color(Select_Screen, lv_color_hex(0xAA0000), LV_PART_MAIN); // ???????
        lv_obj_set_style_text_color(Select_Screen, lv_color_white(), LV_PART_MAIN);
        lv_obj_clear_flag(Select_Screen, LV_OBJ_FLAG_SCROLLABLE);

        lv_obj_t *label = lv_label_create(Select_Screen);
        lv_label_set_text(label, "GAME SELECT!\nTry again!");
        lv_obj_center(label);
    }
}
