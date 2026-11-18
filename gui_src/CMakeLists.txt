my_gui_add_application(my_gui_showcase
    SOURCES showcase.cpp
    STYLING "${CMAKE_CURRENT_SOURCE_DIR}/styling"
    LIBRARIES showcase_backend
)
