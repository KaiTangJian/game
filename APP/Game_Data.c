#include "Game_Data.h"
#include <stddef.h>
#include "stm32f4xx_hal.h"
// Level 1: 简单迷宫，少量障碍，宝石易于获取 (2个宝石: 1火, 1冰)
const uint8_t map_data_level1[MAP_HEIGHT][MAP_WIDTH] =
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 5, 3, 3, 4, 4, 3, 6, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 0, 0, 0, 3, 3, 3, 3, 3, 0, 0, 0, 0, 3, 3, 3, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

// Level 2: 略微复杂的迷宫，引入少量障碍物，宝石需小幅绕行 (3个宝石: 2火, 1冰)
const uint8_t map_data_level2[MAP_HEIGHT][MAP_WIDTH] =
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 0, 0, 0 ,0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 5, 6, 3, 3, 3, 6, 5, 3, 3, 3, 3, 3, 0},
        {0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 3, 0, 0, 0, 0, 3, 3, 0, 0, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 6, 5, 3, 3, 3, 3, 6, 5, 3, 3, 3, 3, 3, 0},
        {0, 0, 0, 3, 3, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 3, 3, 0, 0, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

// Level 3: 更复杂的迷宫，障碍物开始阻挡直接路径，需要更长的路线 (4个宝石: 2火, 2冰)
const uint8_t map_data_level3[MAP_HEIGHT][MAP_WIDTH] =
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 3, 3, 3, 3, 3, 3, 6, 5, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 5, 6, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

// Level 4: 复杂的迷宫，障碍物设置更多，有死路和分叉，宝石位于更具挑战性的位置 (5个宝石: 3火, 2冰)
const uint8_t map_data_level4[MAP_HEIGHT][MAP_WIDTH] =
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0},
        {0, 3, 3, 3, 3, 3, 0, 0, 0, 1, 2, 0, 0, 0, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 6, 0, 0, 3, 3, 0, 0, 3, 3, 0, 0, 5, 3, 3, 3, 0},
        {0, 3, 3, 3, 0, 0, 3, 3, 5, 3, 3, 6, 3, 3, 0, 0, 3, 3, 3, 0},
        {0, 3, 3, 0, 0, 0, 3, 3, 0, 0, 0, 0, 3, 3, 0, 0, 0, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 2, 3, 1, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

// Level 5: 最具挑战性的迷宫，大量障碍物和复杂路径，宝石通常位于危险或难以到达的区域 (8个宝石: 4火, 4冰)
const uint8_t map_data_level5[MAP_HEIGHT][MAP_WIDTH] =
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0,	0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 0, 1, 0, 3, 0, 2, 0, 3, 3, 3, 3, 3, 0},
        {0, 0, 0, 0, 3, 3, 3, 0, 1, 0, 3, 0, 2, 0, 3, 3, 0, 0, 0, 0},
        {0, 3, 3, 3, 3, 3, 3, 0, 1, 0, 3, 0, 2, 0, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 0, 0, 0, 0, 1, 0, 3, 0, 2, 0, 0, 0, 3, 3, 3, 0},
        {0, 4, 3, 3, 3, 3, 3, 0, 1, 0, 3, 0, 2, 0, 3, 3, 3, 3, 4, 0},
        {0, 0, 0, 0, 3, 3, 3, 0, 1, 0, 3, 0, 2, 0, 3, 3, 3, 0, 0, 0},
        {0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 3, 0, 0, 0, 3, 3, 3, 3, 3, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 2, 2, 2, 2, 2, 2, 2, 2, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
const Level_t game_levels[] = {
    {
        .id = 1,
        .map_data = map_data_level1,              // 第一关地图数据
        .player1_start = {.x = 1.0f, .y = 13.0f},  // 冰人起始位置 (左上角)
        .player2_start = {.x = 17.0f, .y = 13.0f}, // 火人起始位置 (右上角)
        .time_limit_sec = 120                     // 2分钟
    },
    {
        .id = 2,
        .map_data = map_data_level2, // 第二关地图数据
        .player1_start = {.x = 1.0f, .y = 1.0f},
        .player2_start = {.x = 13.0f, .y = 1.0f},
        .time_limit_sec = 150 // 2分30秒
    },
    {
        .id = 3,
        .map_data = map_data_level3, // 第三关地图数据
        .player1_start = {.x = 1.0f, .y = 1.0f},
        .player2_start = {.x = 13.0f, .y = 1.0f},
        .time_limit_sec = 180 // 3分钟
    },
    {
        .id = 4,
        .map_data = map_data_level4, // 第四关地图数据
        .player1_start = {.x = 3.0f, .y = 13.0f},
        .player2_start = {.x = 13.0f, .y = 13.0f},
        .time_limit_sec = 210 // 3分30秒
    },
    {
        .id = 5,
        .map_data = map_data_level5, // 第五关地图数据
        .player1_start = {.x = 1.0f, .y = 1.0f},
        .player2_start = {.x = 13.0f, .y = 1.0f},
        .time_limit_sec = 240 // 4分钟
    }};

// Calculate the number of game levels
const uint8_t NUM_GAME_LEVELS = sizeof(game_levels) / sizeof(game_levels[0]);

// Function to get a level by its ID
const Level_t *Level_GetById(uint8_t level_id)
{
    if (level_id > 0 && level_id <= NUM_GAME_LEVELS)
    {
        return &game_levels[level_id - 1]; // Level IDs are 1-based
    }
    return NULL; // Level not found
}

// ... ���еĵ�ͼ���ݺ͹ؿ����� ...

// Flash������غ���ʵ��
/**
 * @brief ��ʼ��Flash�洢
 */
void Flash_Init(void)
{
    // ��ʼ��ʱ���Դ�Flash��ȡ���а�����
    if (!Flash_ReadScores())
    {
        // �����ȡʧ�ܣ�ʹ��Ĭ��ֵ�����浽Flash
        Flash_WriteScores();
    }
}

/**
 * @brief ��Flash��ȡ���а�����
 */
bool Flash_ReadScores(void)
{
    uint32_t *flash_ptr = (uint32_t *)FLASH_STORAGE_ADDRESS;
    uint32_t *data_ptr = (uint32_t *)level_high_scores;
    uint32_t data_size = sizeof(level_high_scores);

    // ���Flash���Ƿ�����Ч���ݣ��򵥼��ǰ4�ֽڲ�Ϊ0xFFFFFFFF��
    if (*flash_ptr == 0xFFFFFFFF)
    {
        return false; // Flash��û������
    }

    // ��Flash��ȡ����
    for (uint32_t i = 0; i < data_size / 4; i++)
    {
        data_ptr[i] = flash_ptr[i];
    }

    return true;
}

/**
 * @brief �����а�����д��Flash
 */
bool Flash_WriteScores(void)
{
    HAL_StatusTypeDef status;
    uint32_t address = FLASH_STORAGE_ADDRESS;
    uint32_t *data_ptr = (uint32_t *)level_high_scores;
    uint32_t data_size = sizeof(level_high_scores);

    // ����Flash
    HAL_FLASH_Unlock();

    // ����Flash����
    FLASH_EraseInitTypeDef eraseInitStruct;
    uint32_t sectorError = 0;

    eraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    eraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    eraseInitStruct.Sector = FLASH_SCORES_SECTOR;
    eraseInitStruct.NbSectors = 1;

    status = HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError);
    if (status != HAL_OK)
    {
        HAL_FLASH_Lock();
        return false;
    }

    // д������ (����д��)
    for (uint32_t i = 0; i < data_size; i += 4)
    {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, *(uint32_t *)((uint8_t *)data_ptr + i));
        if (status != HAL_OK)
        {
            HAL_FLASH_Lock();
            return false;
        }
        address += 4;
    }

    // ����Flash
    HAL_FLASH_Lock();

    return true;
}

/**
 * @brief ����Flash����
 */
void Flash_EraseSector(void)
{
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef eraseInitStruct;
    uint32_t sectorError = 0;

    eraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    eraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    eraseInitStruct.Sector = FLASH_SCORES_SECTOR;
    eraseInitStruct.NbSectors = 1;

    HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError);

    HAL_FLASH_Lock();
}