#ifndef __GAME_MANAGER_H__
#define __GAME_MANAGER_H__
#include "gpio.h"
#include "Game_Data.h"
#include "lvgl.h"
#include "USART_APP.h"
#include "UI_Manager.h"
#include "FreeRTOS.h"
typedef enum
{
    GAME_STATE_LOADING,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_WON,
    GAME_STATE_LOST
} GameState_t;
extern const Level_t* current_level_data;


bool is_move_valid(const GamePlayer_t *player, int8_t dx, int8_t dy);
void Game_HandleInput(uint8_t player_id, int8_t dx, int8_t dy);
bool Game_LoadLevel(uint8_t level_id);
void Game_Update(void);
bool is_gem_collected(uint8_t x, uint8_t y);
#endif
