set(app_src extended_ili9341.c)
set(pre_req lvgl__lvgl espressif__esp_lcd_ili9341 driver log esp_timer atanisoft__esp_lcd_touch_xpt2046)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    REQUIRES ${pre_req})
