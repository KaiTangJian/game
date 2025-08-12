#ifndef UI_Manager_H
#define UI_Manager_H
#include "lvgl.h"
#include "Game_Manager.h"
#include "Game_Data.h"
typedef enum
{
		UI_STATE_START,
		UI_STATE_SELECT,
		UI_STATE_IN_GAMME,
		UI_STATE_EXIT
}UI_STATE_t;

extern UI_STATE_t Current_State;//现在的ui界面
extern int Select_Number;//当前选择的关�?
extern lv_obj_t *Home_Screen;
extern lv_obj_t *Select_Screen;
extern lv_obj_t *Select_Label;
extern lv_obj_t *game_play_screen;
extern lv_obj_t *game_win_screen;
void create_home_screen(void);
void create_game_play_screen(void);
void game_screen_draw_map(const Level_t *level_data);
void game_screen_update_dynamic_elements(const GamePlayer_t *p1, const GamePlayer_t *p2);
void game_screen_update_ui_overlay(uint32_t score, uint32_t p1_health, uint32_t p2_health, uint32_t time_sec);
void create_game_win_screen(void);
void create_game_lose_screen(void);
#endif
