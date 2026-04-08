#ifndef _ENOSE_UI_H
#define _ENOSE_UI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"

#include "ui_helpers.h"
    // #include "ui_events.h"

#define WIFI_START 0x01
#define WIFI_SCAN_DONE 0x02
#define WIFI_FOUND_CHANNEL 0x03
#define WIFI_GOT_SSID_PWD 0x04
#define WIFI_GOT_IP 0x05
#define WIFI_NOT_CONNECTED 0x06
#define WIFI_RECONNECT 0x07

    void appear_Animation(lv_obj_t *TargetObject, int delay);

    // SCREEN: ui_welcomescreen
    void ui_welcomescreen_screen_init(void);
    extern lv_obj_t *ui_welcomescreen;
    void ui_event_devicename(lv_event_t *e);
    extern lv_obj_t *ui_devicename;
    void ui_event_continueContainer(lv_event_t *e);
    extern lv_obj_t *ui_continueContainer;
    extern lv_obj_t *ui_continueLabel;
    void start_continue_Animation(lv_obj_t *TargetObject, int delay);
    // CUSTOM VARIABLES
    extern lv_obj_t *uic_continueContainer;

    // SCREEN: ui_mainscreen
    void ui_mainscreen_screen_init(void);
    // void ui_event_mainscreen(lv_event_t *e);
    void ui_update_device_icon_state(lv_obj_t *TargetObject, bool state);
    void ui_begin_sampling_stage();
    void ui_update_temperature_humidity(float temperature, float humidity);
    extern lv_obj_t *ui_mainscreen;
    extern lv_obj_t *wifi_icon;
    extern lv_obj_t *sdcard_icon;
    extern lv_obj_t *temp_sensor_icon;
    extern lv_obj_t *adc_icon;
    extern lv_obj_t *ui_TempContainer;
    extern lv_obj_t *ui_tempLabel;
    extern lv_obj_t *ui_tempUnit;
    extern lv_obj_t *ui_tempValue;
    extern lv_obj_t *ui_HumContainer;
    extern lv_obj_t *ui_humiLabel;
    extern lv_obj_t *ui_humiUnit;
    extern lv_obj_t *ui_humiValue;
    //  extern lv_obj_t *ui_timelabel;
    void ui_event_measureBTN(lv_event_t *e);
    void ui_reset_before_measure_state();
    void ui_show_measurement_result(const char *result);
    extern lv_obj_t *ui_measureBTN;
    extern lv_obj_t *ui_measureLabel;
    extern lv_obj_t *ui_resultContainer;
    extern lv_obj_t *ui_resultValue;
    extern lv_obj_t *ui_resultLabel;
    extern lv_obj_t *ui_error_notify;
    extern lv_obj_t *ui_titleLabel1;
    extern lv_obj_t *ui_titleLabel2;
    void ui_event_mainscreenMenu(lv_event_t *e);
    extern lv_obj_t *ui_mainscreenMenu;
    extern lv_obj_t *ui_measureState;
    extern lv_obj_t *ui_loadingSpinner;
    void ui_error_notify_screen();
    // CUSTOM VARIABLES

    // SCREEN: ui_aboutscreen
    void
    ui_aboutscreen_screen_init(void);
    extern lv_obj_t *ui_aboutscreen;
    void ui_event_aboutscreenMenu(lv_event_t *e);
    extern lv_obj_t *ui_aboutscreenMenu;
    extern lv_obj_t *ui_abouttitleLabel;
    extern lv_obj_t *ui_about_deviceName;
    extern lv_obj_t *ui_about_deviceID;
    extern lv_obj_t *ui_about_Developer;
    extern lv_obj_t *ui_dev_value;
    extern lv_obj_t *ui_device_name_value;
    extern lv_obj_t *ui_device_ID_value;
    // CUSTOM VARIABLES
    extern lv_obj_t *uic_about_deviceName;
    extern lv_obj_t *uic_Devname_value;
    extern lv_obj_t *uic_device_ID_value;

    // SCREEN: ui_settingscreen
    void ui_settingscreen_screen_init(void);
    extern lv_obj_t *ui_settingscreen;
    void ui_event_settingscreenMenu(lv_event_t *e);
    void ui_event_wifi_setting_onoff(lv_event_t *e);
    void ui_event_heating_setting_onoff(lv_event_t *e);
    void ui_event_pump_setting_onoff(lv_event_t *e);
    void ui_wifi_setting_label_state_change(uint8_t state, char *str);
    void ui_synchronize_pump_state(bool state);
    extern lv_obj_t *ui_settingscreenMenu;
    extern lv_obj_t *ui_settingtitleLabel;
    extern lv_obj_t *ui_wifi_setting_label;
    extern lv_obj_t *ui_wifi_setting_onoff;
    extern lv_obj_t *ui_setting_wifi_state_panel;
    extern lv_obj_t *ui_setting_wifi_state_label;
    extern lv_obj_t *ui_heating_setting_label;
    extern lv_obj_t *ui_heating_setting_onoff;
    extern lv_obj_t *ui_setting_heating_state_panel;
    extern lv_obj_t *ui_setting_heating_state_label;
    extern lv_obj_t *ui_pump_setting_label;
    extern lv_obj_t *ui_pump_setting_onoff;
    extern lv_obj_t *ui_setting_pump_state_panel;
    extern lv_obj_t *ui_setting_pump_state_label;
    // CUSTOM VARIABLES

    // EVENTS
    extern lv_obj_t *ui____initial_actions0;

    // TIMER HANDLE
    void lvgl_timer_handle_task(void *param);

    // UI INIT
    void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
