SET(SOURCES screens/ui_welcomescreen.c
    screens/ui_mainscreen.c
    screens/ui_aboutscreen.c
    screens/ui_settingscreen.c
    ui.c
    ui_helpers.c)

add_library(ui ${SOURCES})
