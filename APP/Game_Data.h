#ifndef __Game_Data_H__
#define __Game_Data_H__

#include <stdint.h>
#include <stdbool.h> // For bool type
#include "stm32f4xx_hal.h"
// Define your map dimensions
#define MAP_WIDTH   20
#define MAP_HEIGHT  15
#define TILE_SIZE 5
// 排行榜相关定义
#define TOTAL_LEVELS 5
#define MAX_SCORES_PER_LEVEL 5
#define FLASH_STORAGE_ADDRESS 0x080E0000  // 使用Sector 11 (最后128KB扇区中的前16KB)
#define FLASH_SCORES_SECTOR FLASH_SECTOR_11
#define SCORES_DATA_SIZE (sizeof(level_high_scores))
extern uint32_t level_high_scores[TOTAL_LEVELS][MAX_SCORES_PER_LEVEL];



// Flash操作函数声明
void Flash_Init(void);
bool Flash_ReadScores(void);
bool Flash_WriteScores(void);
void Flash_ErasePage(void);
// Tile Types (Adjust based on your game mechanics)
typedef enum {
    TILE_TYPE_WALL = 0,//墙壁
    TILE_TYPE_FIRE,     // 火焰
    TILE_TYPE_ICE,      // 冰川
    TILE_TYPE_NORMAL,   // 正常地面
    TILE_TYPE_EXIT,     // 出口
    TILE_TYPE_COLLECTIBLE_FIRE_GEM, // 火宝
    TILE_TYPE_COLLECTIBLE_ICE_GEM,  // 冰宝
    TILE_TYPE_DOOR,     // 待打开的出口
    
} TileType_t;


//角色数据
typedef struct 
{
    float x;
    float y;
} Point_t;

typedef enum 
{
    PLAYER_TYPE_ICE,
    PLAYER_TYPE_FIRE
} PlayerType_t;

typedef struct GamePlayer 
{
    Point_t pos;           // 当前位置 (浮点坐标，用于平滑移动和物理计算)
    PlayerType_t type;     // 冰人还是火人
    int health;            // 生命�? (例如�?100�?)
    uint32_t score;        // 当前分数
    uint8_t lives;         // 剩余生命�?
    bool is_alive;         // 是否存活
    bool is_invincible;    // 是否处于无敌状�?
    float vertical_velocity; // 当前垂直速度 (负数向上，正数向�?)
    bool on_ground;          // 是否站在地面�?
    bool is_jumping;         // 是否正在跳跃
} GamePlayer_t;
// 关卡设置
typedef struct {
    uint8_t id;
    const uint8_t (*map_data)[MAP_WIDTH]; // Pointer to the 2D map data array (saves Flash)
    Point_t player1_start; // 冰人开�?
    Point_t player2_start; // 火人开�?
    uint16_t time_limit_sec;
    // ... other level specific data
} Level_t;

// Extern declaration for game levels array
extern const Level_t game_levels[];
extern const uint8_t NUM_GAME_LEVELS;

// Function to get level by ID
const Level_t* Level_GetById(uint8_t level_id);

#endif 
