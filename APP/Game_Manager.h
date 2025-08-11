#ifndef __GAME_MANAGER_H__
#define __GAME_MANAGER_H__
#include "gpio.h"
#include "Game_Data.h"
#include "lvgl.h"

//extern GameState_t current_game_state;
//extern const Level_t* current_level_data;

//extern GamePlayer_t current_player1_state; // 冰人的当前状态
//extern GamePlayer_t current_player2_state; // 火人的当前状态

//extern uint32_t current_game_score;
//extern uint32_t remaining_game_time_sec;
bool is_move_valid(const GamePlayer_t *player, int8_t dx, int8_t dy);
void Game_HandleInput(uint8_t player_id, int8_t dx, int8_t dy);
bool Game_LoadLevel(uint8_t level_id);
void Game_Update(void);
#endif