#include "../ui.h"

void ui_aboutscreen_screen_init(void)
{
    ui_aboutscreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_aboutscreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_aboutscreen, lv_color_hex(0xFFEFBA), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_aboutscreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_aboutscreen, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui_aboutscreen, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui_aboutscreen, 179, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_aboutscreen, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_aboutscreenMenu = lv_dropdown_create(ui_aboutscreen);
    lv_dropdown_set_dir(ui_aboutscreenMenu, LV_DIR_LEFT);
    lv_dropdown_set_options(ui_aboutscreenMenu, "Main\nSetting");
    lv_dropdown_set_selected_highlight(ui_aboutscreenMenu, false);
    lv_dropdown_set_text(ui_aboutscreenMenu, "Menu");
    lv_obj_set_width(ui_aboutscreenMenu, 62);
    lv_obj_set_height(ui_aboutscreenMenu, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_aboutscreenMenu, 85);
    lv_obj_set_y(ui_aboutscreenMenu, 3);
    lv_obj_set_align(ui_aboutscreenMenu, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_aboutscreenMenu, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_style_text_font(ui_aboutscreenMenu, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_aboutscreenMenu, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_aboutscreenMenu, lv_color_hex(0xFFE282), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_aboutscreenMenu, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_aboutscreenMenu, lv_color_hex(0xBDB395), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_aboutscreenMenu, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_aboutscreenMenu, 1, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_font(ui_aboutscreenMenu, &lv_font_montserrat_10, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_text_color(lv_dropdown_get_list(ui_aboutscreenMenu), lv_color_hex(0x808080),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(lv_dropdown_get_list(ui_aboutscreenMenu), 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_abouttitleLabel = lv_label_create(ui_aboutscreen);
    lv_obj_set_width(ui_abouttitleLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_abouttitleLabel, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_abouttitleLabel, 0);
    lv_obj_set_y(ui_abouttitleLabel, 25);
    lv_obj_set_align(ui_abouttitleLabel, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_abouttitleLabel, "ABOUT");
    lv_obj_set_style_text_color(ui_abouttitleLabel, lv_color_hex(0x564F4F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_abouttitleLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_abouttitleLabel, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_about_deviceName = lv_label_create(ui_aboutscreen);
    lv_obj_set_width(ui_about_deviceName, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_about_deviceName, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_about_deviceName, 5);
    lv_obj_set_y(ui_about_deviceName, 60);
    lv_label_set_text(ui_about_deviceName, "Device name:");
    lv_obj_set_style_text_color(ui_about_deviceName, lv_color_hex(0x564F4F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_about_deviceName, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_about_deviceID = lv_label_create(ui_aboutscreen);
    lv_obj_set_width(ui_about_deviceID, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_about_deviceID, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_about_deviceID, 5);
    lv_obj_set_y(ui_about_deviceID, 95);
    lv_label_set_text(ui_about_deviceID, "Device ID:");
    lv_obj_set_style_text_color(ui_about_deviceID, lv_color_hex(0x564F4F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_about_deviceID, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_about_Developer = lv_label_create(ui_aboutscreen);
    lv_obj_set_width(ui_about_Developer, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_about_Developer, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_about_Developer, 5);
    lv_obj_set_y(ui_about_Developer, 130);
    lv_label_set_text(ui_about_Developer, "Developers:");
    lv_obj_set_style_text_color(ui_about_Developer, lv_color_hex(0x564F4F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_about_Developer, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_dev_value = lv_label_create(ui_aboutscreen);
    lv_obj_set_width(ui_dev_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_dev_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_dev_value, 0);
    lv_obj_set_y(ui_dev_value, 147);
    lv_obj_set_align(ui_dev_value, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_dev_value, "- SPARC Lab\n- Assoc. Prof Nguyen Minh Tan's team");
    lv_obj_set_style_text_color(ui_dev_value, lv_color_hex(0xF50000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_dev_value, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_dev_value, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_device_name_value = lv_label_create(ui_aboutscreen);
    lv_obj_set_width(ui_device_name_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_device_name_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_device_name_value, 0);
    lv_obj_set_y(ui_device_name_value, 77);
    lv_obj_set_align(ui_device_name_value, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_device_name_value, "E-NOSE for Milk Quality Evaluation");
    lv_obj_set_style_text_color(ui_device_name_value, lv_color_hex(0xF50000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_device_name_value, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_device_name_value, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_device_ID_value = lv_label_create(ui_aboutscreen);
    lv_obj_set_width(ui_device_ID_value, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_device_ID_value, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_device_ID_value, 0);
    lv_obj_set_y(ui_device_ID_value, 112);
    lv_obj_set_align(ui_device_ID_value, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_device_ID_value, "EN-SPARC-01");
    lv_obj_set_style_text_color(ui_device_ID_value, lv_color_hex(0xF50000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_device_ID_value, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_device_ID_value, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_aboutscreenMenu, ui_event_aboutscreenMenu, LV_EVENT_ALL, NULL);
    uic_about_deviceName = ui_about_deviceName;
    uic_Devname_value = ui_dev_value;
    uic_device_ID_value = ui_device_ID_value;
}
