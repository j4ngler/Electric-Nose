#include "../ui.h"

void ui_settingscreen_screen_init(void)
{
    ui_settingscreen = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_settingscreen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_settingscreen, lv_color_hex(0xFFEFBA), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_settingscreen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_settingscreen, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui_settingscreen, 100, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui_settingscreen, 179, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_settingscreen, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settingscreenMenu = lv_dropdown_create(ui_settingscreen);
    lv_dropdown_set_dir(ui_settingscreenMenu, LV_DIR_LEFT);
    lv_dropdown_set_options(ui_settingscreenMenu, "Main\nAbout");
    lv_dropdown_set_selected_highlight(ui_settingscreenMenu, false);
    lv_dropdown_set_text(ui_settingscreenMenu, "Menu");
    lv_obj_set_width(ui_settingscreenMenu, 62);
    lv_obj_set_height(ui_settingscreenMenu, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_settingscreenMenu, 85);
    lv_obj_set_y(ui_settingscreenMenu, 3);
    lv_obj_set_align(ui_settingscreenMenu, LV_ALIGN_TOP_MID);
    lv_obj_add_flag(ui_settingscreenMenu, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_set_style_text_font(ui_settingscreenMenu, &lv_font_montserrat_10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_settingscreenMenu, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_settingscreenMenu, lv_color_hex(0xFFE282), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_settingscreenMenu, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_settingscreenMenu, lv_color_hex(0xBDB395), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_settingscreenMenu, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_settingscreenMenu, 1, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_font(ui_settingscreenMenu, &lv_font_montserrat_10, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_text_color(lv_dropdown_get_list(ui_settingscreenMenu), lv_color_hex(0x808080),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(lv_dropdown_get_list(ui_settingscreenMenu), 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_settingtitleLabel = lv_label_create(ui_settingscreen);
    lv_obj_set_width(ui_settingtitleLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_settingtitleLabel, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_settingtitleLabel, 0);
    lv_obj_set_y(ui_settingtitleLabel, 25);
    lv_obj_set_align(ui_settingtitleLabel, LV_ALIGN_TOP_MID);
    lv_label_set_text(ui_settingtitleLabel, "SETTING");
    lv_obj_set_style_text_color(ui_settingtitleLabel, lv_color_hex(0x564F4F), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_settingtitleLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_settingtitleLabel, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_wifi_setting_label = lv_label_create(ui_settingscreen);
    lv_obj_set_width(ui_wifi_setting_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_wifi_setting_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_wifi_setting_label, 8);
    lv_obj_set_y(ui_wifi_setting_label, 57);
    lv_obj_set_align(ui_wifi_setting_label, LV_ALIGN_TOP_LEFT);
    lv_label_set_text(ui_wifi_setting_label, "Wi-Fi:");

    ui_wifi_setting_onoff = lv_switch_create(ui_settingscreen);
    lv_obj_set_width(ui_wifi_setting_onoff, 30);
    lv_obj_set_height(ui_wifi_setting_onoff, 15);
    lv_obj_set_x(ui_wifi_setting_onoff, 68);
    lv_obj_set_y(ui_wifi_setting_onoff, 57);
    lv_obj_set_align(ui_wifi_setting_onoff, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_border_color(ui_wifi_setting_onoff, lv_color_hex(0xBDB395), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_wifi_setting_onoff, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_wifi_setting_onoff, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_wifi_setting_onoff, lv_color_hex(0x34C759), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui_wifi_setting_onoff, 255, LV_PART_INDICATOR | LV_STATE_CHECKED);

    ui_setting_wifi_state_panel = lv_obj_create(ui_settingscreen);
    lv_obj_set_width(ui_setting_wifi_state_panel, 225);
    lv_obj_set_height(ui_setting_wifi_state_panel, 23);
    lv_obj_set_x(ui_setting_wifi_state_panel, 0);
    lv_obj_set_y(ui_setting_wifi_state_panel, -74);
    lv_obj_set_align(ui_setting_wifi_state_panel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_setting_wifi_state_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_setting_wifi_state_panel, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_setting_wifi_state_panel, lv_color_hex(0xDBDBDB), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_setting_wifi_state_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_setting_wifi_state_panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_setting_wifi_state_panel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_setting_wifi_state_label = lv_label_create(ui_setting_wifi_state_panel);
    lv_obj_set_width(ui_setting_wifi_state_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_setting_wifi_state_label, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_setting_wifi_state_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_setting_wifi_state_label, "Not connected");
    lv_obj_set_style_text_color(ui_setting_wifi_state_label, lv_color_hex(0xd92626), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_setting_wifi_state_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_setting_wifi_state_label, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_heating_setting_label = lv_label_create(ui_settingscreen);
    lv_obj_set_width(ui_heating_setting_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_heating_setting_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_heating_setting_label, 8);
    lv_obj_set_y(ui_heating_setting_label, 105);
    lv_obj_set_align(ui_wifi_setting_label, LV_ALIGN_TOP_LEFT);
    lv_label_set_text(ui_heating_setting_label, "Heating sensors:");

    ui_setting_heating_state_panel = lv_obj_create(ui_settingscreen);
    lv_obj_remove_style_all(ui_setting_heating_state_panel);
    lv_obj_set_width(ui_setting_heating_state_panel, 225);
    lv_obj_set_height(ui_setting_heating_state_panel, 23);
    lv_obj_set_x(ui_setting_heating_state_panel, 0);
    lv_obj_set_y(ui_setting_heating_state_panel, -26);
    lv_obj_set_align(ui_setting_heating_state_panel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_setting_heating_state_panel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_setting_heating_state_panel, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_setting_heating_state_panel, lv_color_hex(0xDBDBDB), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_setting_heating_state_panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_setting_heating_state_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_setting_heating_state_panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_setting_heating_state_panel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_setting_heating_state_label = lv_label_create(ui_setting_heating_state_panel);
    lv_obj_set_width(ui_setting_heating_state_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_setting_heating_state_label, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_setting_heating_state_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_setting_heating_state_label, "Turned on");
    lv_obj_set_style_text_color(ui_setting_heating_state_label, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_setting_heating_state_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_setting_heating_state_label, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_heating_setting_onoff = lv_switch_create(ui_settingscreen);
    lv_obj_set_width(ui_heating_setting_onoff, 30);
    lv_obj_set_height(ui_heating_setting_onoff, 15);
    lv_obj_set_x(ui_heating_setting_onoff, 140);
    lv_obj_set_y(ui_heating_setting_onoff, 105);
    lv_obj_set_align(ui_heating_setting_onoff, LV_ALIGN_TOP_LEFT);
    lv_obj_add_state(ui_heating_setting_onoff, LV_STATE_CHECKED);
    lv_obj_set_style_border_color(ui_heating_setting_onoff, lv_color_hex(0xBDB395), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_heating_setting_onoff, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_heating_setting_onoff, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_heating_setting_onoff, lv_color_hex(0x34C759), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui_heating_setting_onoff, 255, LV_PART_INDICATOR | LV_STATE_CHECKED);

    ui_pump_setting_label = lv_label_create(ui_settingscreen);
    lv_obj_set_width(ui_pump_setting_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_pump_setting_label, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_pump_setting_label, 8);
    lv_obj_set_y(ui_pump_setting_label, 153);
    lv_obj_set_align(ui_pump_setting_label, LV_ALIGN_TOP_LEFT);
    lv_label_set_text(ui_pump_setting_label, "Air pump:");

    ui_setting_pump_state_panel = lv_obj_create(ui_settingscreen);
    lv_obj_remove_style_all(ui_setting_pump_state_panel);
    lv_obj_set_width(ui_setting_pump_state_panel, 225);
    lv_obj_set_height(ui_setting_pump_state_panel, 23);
    lv_obj_set_x(ui_setting_pump_state_panel, 0);
    lv_obj_set_y(ui_setting_pump_state_panel, 22);
    lv_obj_set_align(ui_setting_pump_state_panel, LV_ALIGN_CENTER);
    lv_obj_clear_flag(ui_setting_pump_state_panel, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_setting_pump_state_panel, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_setting_pump_state_panel, lv_color_hex(0xDBDBDB), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_setting_pump_state_panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_setting_pump_state_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_setting_pump_state_panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_setting_pump_state_panel, 1, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_setting_pump_state_label = lv_label_create(ui_setting_pump_state_panel);
    lv_obj_set_width(ui_setting_pump_state_label, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_setting_pump_state_label, LV_SIZE_CONTENT);
    lv_obj_set_align(ui_setting_pump_state_label, LV_ALIGN_CENTER);
    lv_label_set_text(ui_setting_pump_state_label, "Turned off");
    lv_obj_set_style_text_color(ui_setting_pump_state_label, lv_color_hex(0xd92626), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_setting_pump_state_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_setting_pump_state_label, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_pump_setting_onoff = lv_switch_create(ui_settingscreen);
    lv_obj_set_width(ui_pump_setting_onoff, 30);
    lv_obj_set_height(ui_pump_setting_onoff, 15);
    lv_obj_set_x(ui_pump_setting_onoff, 140);
    lv_obj_set_y(ui_pump_setting_onoff, 153);
    lv_obj_set_align(ui_pump_setting_onoff, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_border_color(ui_pump_setting_onoff, lv_color_hex(0xBDB395), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_pump_setting_onoff, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_pump_setting_onoff, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_pump_setting_onoff, lv_color_hex(0x34C759), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(ui_pump_setting_onoff, 255, LV_PART_INDICATOR | LV_STATE_CHECKED);

    lv_obj_add_event_cb(ui_wifi_setting_onoff, ui_event_wifi_setting_onoff, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui_heating_setting_onoff, ui_event_heating_setting_onoff, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui_pump_setting_onoff, ui_event_pump_setting_onoff, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui_settingscreenMenu, ui_event_settingscreenMenu, LV_EVENT_ALL, NULL);
}
