#include "../ui.h"

void ui_mainscreen_screen_init(void)
{
    ui_mainscreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_mainscreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_mainscreen, lv_color_hex(0xFFDF71), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_mainscreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_mainscreen, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui_mainscreen, 79, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui_mainscreen, 179, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_mainscreen, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);

    wifi_icon = lv_label_create(ui_mainscreen);
    lv_obj_align(wifi_icon, LV_ALIGN_TOP_LEFT, 6, 4);
    lv_label_set_text(wifi_icon, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_color(wifi_icon, lv_color_hex(0xd92626), LV_PART_MAIN);
    lv_obj_set_style_text_font(wifi_icon, &lv_font_montserrat_14, LV_PART_MAIN);

    sdcard_icon = lv_label_create(ui_mainscreen);
    lv_obj_align(sdcard_icon, LV_ALIGN_TOP_LEFT, 30, 4);
    lv_label_set_text(sdcard_icon, LV_SYMBOL_SD_CARD);
    lv_obj_set_style_text_color(sdcard_icon, lv_color_hex(0xd92626), LV_PART_MAIN);
    lv_obj_set_style_text_font(sdcard_icon, &lv_font_montserrat_14, LV_PART_MAIN);

    temp_sensor_icon = lv_label_create(ui_mainscreen);
    lv_obj_align(temp_sensor_icon, LV_ALIGN_TOP_LEFT, 46, 4);
    lv_label_set_text(temp_sensor_icon, "°C");
    lv_obj_set_style_text_color(temp_sensor_icon, lv_color_hex(0xd92626), LV_PART_MAIN);
    lv_obj_set_style_text_font(temp_sensor_icon, &lv_font_montserrat_14, LV_PART_MAIN);

    adc_icon = lv_label_create(ui_mainscreen);
    lv_obj_align(adc_icon, LV_ALIGN_TOP_LEFT, 68, 4);
    lv_label_set_text(adc_icon, LV_SYMBOL_DOWNLOAD);
    lv_obj_set_style_text_color(adc_icon, lv_color_hex(0xd92626), LV_PART_MAIN);
    lv_obj_set_style_text_font(adc_icon, &lv_font_montserrat_14, LV_PART_MAIN);

    ui_TempContainer = lv_obj_create(ui_mainscreen);
    lv_obj_remove_style_all(ui_TempContainer);
    lv_obj_set_width(ui_TempContainer, 110);
    lv_obj_set_height(ui_TempContainer, 90);
    lv_obj_set_x(ui_TempContainer, -60);
    lv_obj_set_y(ui_TempContainer, 70);
    lv_obj_set_align(ui_TempContainer, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_TempContainer, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_TempContainer, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_TempContainer, lv_color_hex(0x7A7A7A), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_TempContainer, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_TempContainer, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_TempContainer, 96, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_TempContainer, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui_TempContainer, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_TempContainer, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_TempContainer, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_TempContainer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_x(ui_TempContainer, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_y(ui_TempContainer, 4, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_tempLabel = lv_label_create(ui_TempContainer);
    lv_obj_set_width(ui_tempLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_tempLabel, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_tempLabel, 0);
    lv_obj_set_y(ui_tempLabel, 1);
    lv_obj_set_align(ui_tempLabel, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_tempLabel, "Temperature");
    lv_obj_set_style_text_color(ui_tempLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_tempLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_tempLabel, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_tempUnit = lv_label_create(ui_TempContainer);
    lv_obj_set_width(ui_tempUnit, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_tempUnit, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_tempUnit, 0);
    lv_obj_set_y(ui_tempUnit, -20);
    lv_obj_set_align(ui_tempUnit, LV_ALIGN_CENTER);
    lv_label_set_text(ui_tempUnit, "(°C)");
    lv_obj_set_style_text_color(ui_tempUnit, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_tempUnit, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_tempValue = lv_label_create(ui_TempContainer);
    lv_obj_set_width(ui_tempValue, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_tempValue, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_tempValue, 0);
    lv_obj_set_y(ui_tempValue, 5);
    lv_obj_set_align(ui_tempValue, LV_ALIGN_CENTER);
    lv_label_set_text(ui_tempValue, "0.00");
    lv_obj_set_style_text_color(ui_tempValue, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_tempValue, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_HumContainer = lv_obj_create(ui_mainscreen);
    lv_obj_remove_style_all(ui_HumContainer);
    lv_obj_set_width(ui_HumContainer, 110);
    lv_obj_set_height(ui_HumContainer, 90);
    lv_obj_set_x(ui_HumContainer, 60);
    lv_obj_set_y(ui_HumContainer, 70);
    lv_obj_set_align(ui_HumContainer, LV_ALIGN_TOP_MID);
    lv_obj_clear_flag(ui_HumContainer, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_HumContainer, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_HumContainer, lv_color_hex(0x7A7A7A), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_HumContainer, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_HumContainer, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_HumContainer, 96, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_HumContainer, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui_HumContainer, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_HumContainer, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_HumContainer, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_HumContainer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_x(ui_HumContainer, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_ofs_y(ui_HumContainer, 4, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_humiLabel = lv_label_create(ui_HumContainer);
    lv_obj_set_width(ui_humiLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_humiLabel, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_humiLabel, 0);
    lv_obj_set_y(ui_humiLabel, 1);
    lv_obj_set_align(ui_humiLabel, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_humiLabel, "Humidity");
    lv_obj_set_style_text_color(ui_humiLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_humiLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_humiLabel, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_humiUnit = lv_label_create(ui_HumContainer);
    lv_obj_set_width(ui_humiUnit, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_humiUnit, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_humiUnit, 0);
    lv_obj_set_y(ui_humiUnit, -20);
    lv_obj_set_align(ui_humiUnit, LV_ALIGN_CENTER);
    lv_label_set_text(ui_humiUnit, "(%)");
    lv_obj_set_style_text_color(ui_humiUnit, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_humiUnit, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_humiValue = lv_label_create(ui_HumContainer);
    lv_obj_set_width(ui_humiValue, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_humiValue, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_humiValue, 0);
    lv_obj_set_y(ui_humiValue, 5);
    lv_obj_set_align(ui_humiValue, LV_ALIGN_CENTER);
    lv_label_set_text(ui_humiValue, "0");
    lv_obj_set_style_text_color(ui_humiValue, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_humiValue, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_timelabel = lv_label_create(ui_mainscreen);
    // lv_obj_set_width(ui_timelabel, LV_SIZE_CONTENT);
    // lv_obj_set_height(ui_timelabel, LV_SIZE_CONTENT);
    // lv_obj_set_x(ui_timelabel, 0);
    // lv_obj_set_y(ui_timelabel, 1);
    // lv_obj_set_align(ui_timelabel, LV_ALIGN_TOP_MID);
    // lv_label_set_text(ui_timelabel, "00:00");
    // lv_obj_set_style_text_font(ui_timelabel, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_measureBTN = lv_btn_create(ui_mainscreen);
    lv_obj_set_width(ui_measureBTN, 170);
    lv_obj_set_height(ui_measureBTN, 25);
    lv_obj_set_x(ui_measureBTN, 0);
    lv_obj_set_y(ui_measureBTN, 30);
    lv_obj_set_align(ui_measureBTN, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_measureBTN, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_measureBTN, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_measureBTN, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_measureBTN, lv_color_hex(0xEACDA3), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_measureBTN, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_measureBTN, lv_color_hex(0xD6AE7B), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_measureBTN, LV_GRAD_DIR_HOR, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_measureBTN, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_measureBTN, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_measureBTN, 1, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_measureLabel = lv_label_create(ui_measureBTN);
    lv_obj_set_width(ui_measureLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_measureLabel, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_measureLabel, LV_ALIGN_CENTER);
    lv_label_set_text(ui_measureLabel, "Start measurement");
    lv_obj_set_style_text_color(ui_measureLabel, lv_color_hex(0x564F4F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_measureLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_measureLabel, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_resultContainer = lv_obj_create(ui_mainscreen);
    lv_obj_remove_style_all(ui_resultContainer);
    lv_obj_set_width(ui_resultContainer, 226);
    lv_obj_set_height(ui_resultContainer, 65);
    lv_obj_set_x(ui_resultContainer, 0);
    lv_obj_set_y(ui_resultContainer, -20);
    lv_obj_set_align(ui_resultContainer, LV_ALIGN_BOTTOM_MID);
    lv_obj_clear_flag(ui_resultContainer, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_resultContainer, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_resultContainer, lv_color_hex(0xF6F0F0), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_resultContainer, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_resultContainer, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_resultContainer, 190, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_resultContainer, 1, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_resultValue = lv_label_create(ui_resultContainer);
    lv_obj_set_width(ui_resultValue, 200);
    lv_obj_set_height(ui_resultValue, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_resultValue, LV_ALIGN_CENTER);
    lv_obj_set_style_text_align(ui_resultValue, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_label_set_text(ui_resultValue, "");
    lv_obj_set_style_text_font(ui_resultValue, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_add_flag(ui_resultValue, LV_OBJ_FLAG_HIDDEN);

    ui_error_notify = lv_label_create(ui_mainscreen);
    lv_obj_set_width(ui_error_notify, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_error_notify, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_error_notify, 0);
    lv_obj_set_y(ui_error_notify, -5);
    lv_obj_set_align(ui_error_notify, LV_ALIGN_BOTTOM_MID);
    lv_label_set_text(ui_error_notify, "Modules get errors! Please reset your device");
    lv_obj_set_style_text_color(ui_error_notify, lv_color_hex(0xd92626), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_error_notify, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_error_notify, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_flag(ui_error_notify, LV_OBJ_FLAG_HIDDEN);

    ui_resultLabel = lv_label_create(ui_mainscreen);
    lv_obj_set_width(ui_resultLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_resultLabel, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_resultLabel, 5);
    lv_obj_set_y(ui_resultLabel, 60);
    lv_obj_set_align(ui_resultLabel, LV_ALIGN_LEFT_MID);
    lv_label_set_text(ui_resultLabel, "Status:");
    lv_obj_set_style_text_color(ui_resultLabel, lv_color_hex(0x564F4F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_resultLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_titleLabel1 = lv_label_create(ui_mainscreen);
    lv_obj_set_width(ui_titleLabel1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_titleLabel1, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_titleLabel1, 0);
    lv_obj_set_y(ui_titleLabel1, 25);
    lv_obj_set_align(ui_titleLabel1, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_titleLabel1, "E-NOSE");
    lv_obj_set_style_text_color(ui_titleLabel1, lv_color_hex(0x564F4F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_titleLabel1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_titleLabel1, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_titleLabel2 = lv_label_create(ui_mainscreen);
    lv_obj_set_width(ui_titleLabel2, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_titleLabel2, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_titleLabel2, 0);
    lv_obj_set_y(ui_titleLabel2, 45);
    lv_obj_set_align(ui_titleLabel2, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_titleLabel2, "Milk quality evaluation");

    ui_mainscreenMenu = lv_dropdown_create(ui_mainscreen);
    lv_dropdown_set_dir(ui_mainscreenMenu, LV_DIR_LEFT);
    lv_dropdown_set_options(ui_mainscreenMenu, "About\nSetting");
    lv_dropdown_set_selected_highlight(ui_mainscreenMenu, false);
    lv_dropdown_set_text(ui_mainscreenMenu, "Menu");
    lv_obj_set_width(ui_mainscreenMenu, 62);
    lv_obj_set_height(ui_mainscreenMenu, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_mainscreenMenu, 85);
    lv_obj_set_y(ui_mainscreenMenu, 3);
    lv_obj_set_align(ui_mainscreenMenu, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_mainscreenMenu, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_style_text_font(ui_mainscreenMenu, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_mainscreenMenu, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_mainscreenMenu, lv_color_hex(0xFFE282), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_mainscreenMenu, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_mainscreenMenu, lv_color_hex(0xBDB395), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_mainscreenMenu, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_mainscreenMenu, 1, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_font(ui_mainscreenMenu, &lv_font_montserrat_10, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_text_color(lv_dropdown_get_list(ui_mainscreenMenu), lv_color_hex(0x808080),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(lv_dropdown_get_list(ui_mainscreenMenu), 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_measureState = lv_label_create(ui_mainscreen);
    lv_obj_set_width(ui_measureState, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_measureState, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_measureState, 5);
    lv_obj_set_y(ui_measureState, 60);
    lv_obj_set_align(ui_measureState, LV_ALIGN_CENTER);
    lv_label_set_text(ui_measureState, "Sampling");
    lv_obj_add_flag(ui_measureState, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_text_color(ui_measureState, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_measureState, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_loadingSpinner = lv_spinner_create(ui_mainscreen, 1000, 90);
    lv_obj_set_width(ui_loadingSpinner, 20);
    lv_obj_set_height(ui_loadingSpinner, 20);
    lv_obj_set_x(ui_loadingSpinner, 90);
    lv_obj_set_y(ui_loadingSpinner, 60);
    lv_obj_set_align(ui_loadingSpinner, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_loadingSpinner, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_loadingSpinner, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(ui_loadingSpinner, 3, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(ui_loadingSpinner, lv_color_hex(0x564F4F), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_loadingSpinner, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_loadingSpinner, 3, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_measureBTN, ui_event_measureBTN, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_mainscreenMenu, ui_event_mainscreenMenu, LV_EVENT_ALL, NULL);
    // lv_obj_add_event_cb(ui_mainscreen, ui_event_mainscreen, LV_EVENT_SCREEN_LOADED, NULL);
}
