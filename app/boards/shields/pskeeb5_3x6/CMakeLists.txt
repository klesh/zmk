if(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../pskeeb5/custom_status_screen.c)
endif()
