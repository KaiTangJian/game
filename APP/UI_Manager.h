#ifndef UI_Manager_H
#define UI_Manager_H
#include "lvgl.h"
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
void create_home_screen(void);




#endif