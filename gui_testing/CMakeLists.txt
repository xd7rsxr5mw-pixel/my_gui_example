find_package(Qt6 6.9 REQUIRED COMPONENTS Test)
set(CMAKE_AUTOMOC ON)

qt_add_executable(mygui_example_backend_tests cpp/tst_Backend.cpp)
target_link_libraries(mygui_example_backend_tests PRIVATE showcase_backend Qt6::Test)
add_test(NAME mygui.example.backend COMMAND mygui_example_backend_tests)

my_gui_add_application(mygui_example_startup SOURCES ../gui_src/showcase.cpp
    STYLING "${PROJECT_SOURCE_DIR}/gui_src/styling" LIBRARIES showcase_backend
    OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/showcase")
add_test(NAME mygui.example.startup COMMAND mygui_example_startup
    --no-persist --data-root "${CMAKE_CURRENT_BINARY_DIR}/showcase-data" --quit-after 1000)
set_tests_properties(mygui.example.startup PROPERTIES
    FAIL_REGULAR_EXPRESSION "Binding loop;TypeError;ReferenceError;Cannot assign;negative cache;unavailable"
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QUICK_CONTROLS_STYLE=Basic;QT_QUICK_BACKEND=software;QT_FORCE_STDERR_LOGGING=1"
    TIMEOUT 30)
