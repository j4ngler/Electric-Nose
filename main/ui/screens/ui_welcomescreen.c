#include "../ui.h"

void ui_welcomescreen_screen_init(void)
{
    ui_welcomescreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_welcomescreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_welcomescreen, lv_color_hex(0xFFDF71), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_welcomescreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_welcomescreen, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui_welcomescreen, 79, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui_welcomescreen, 179, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_welcomescreen, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_devicename = lv_label_create(ui_welcomescreen);
    lv_obj_set_width(ui_devicename, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_devicename, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_devicename, 0);
    lv_obj_set_y(ui_devicename, -33);
    lv_obj_set_align(ui_devicename, LV_ALIGN_CENTER);
    lv_label_set_text(ui_devicename, "E-NOSE");
    lv_obj_set_style_text_color(ui_devicename, lv_color_hex(0x564F4F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_devicename, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_devicename, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_devicename, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_devicename, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_devicename, 2, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_continueContainer = lv_obj_create(ui_welcomescreen);
    lv_obj_remove_style_all(ui_continueContainer);
    lv_obj_set_width(ui_continueContainer, 240);
    lv_obj_set_height(ui_continueContainer, 50);
    lv_obj_set_x(ui_continueContainer, 0);
    lv_obj_set_y(ui_continueContainer, -30);
    lv_obj_set_align(ui_continueContainer, LV_ALIGN_BOTTOM_MID);
    lv_obj_clear_flag(ui_continueContainer, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    ui_continueLabel = lv_label_create(ui_continueContainer);
    lv_obj_set_width(ui_continueLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_continueLabel, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_continueLabel, LV_ALIGN_CENTER);
    lv_obj_set_style_text_opa(ui_continueLabel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(ui_continueLabel, "Click here to continue");

    lv_obj_add_event_cb(ui_welcomescreen, ui_event_devicename, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_continueContainer, ui_event_continueContainer, LV_EVENT_ALL, NULL);

    uic_continueContainer = ui_continueContainer;
}
