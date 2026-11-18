add_library(showcase_backend STATIC calculator.cpp)
target_include_directories(showcase_backend PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
set_target_properties(showcase_backend PROPERTIES AUTOMOC OFF AUTOUIC OFF AUTORCC OFF)
