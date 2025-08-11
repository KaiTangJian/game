#include "UI_Manager.h"



UI_STATE_t Current_State = UI_STATE_START;//现在的ui界面
int Select_Number = 1;//当前选择的关卡
lv_obj_t *Home_Screen;
lv_obj_t *Select_Screen;
lv_obj_t *Select_Label;

void create_home_screen(void) 
{
    Home_Screen = lv_obj_create(NULL); // 创建一个新屏幕
    lv_obj_set_style_bg_color(Home_Screen, lv_color_hex(0xFFFF), 0);

    lv_obj_t *title = lv_label_create(Home_Screen);
    lv_label_set_text(title, "FIRE AND ICE");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 50);
    //lv_obj_set_style_text_color(title, lv_color_hex(0xFF0000), 0); // 红色文字
    lv_obj_set_style_text_color(title, lv_color_hex(0xF800), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0); // 24号字体，可根据需要调整
    lv_obj_t *hint_start = lv_label_create(Home_Screen);
    lv_label_set_text(hint_start, "Buttom1:Enter");
    lv_obj_align(hint_start, LV_ALIGN_TOP_LEFT, 10, 10);

    lv_obj_t *hint_exit = lv_label_create(Home_Screen);
    lv_label_set_text(hint_exit, "Buttom2:Exit");
    lv_obj_align_to(hint_exit, hint_start, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
}






