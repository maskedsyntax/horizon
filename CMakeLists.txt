cmake_minimum_required(VERSION 3.16)

project(Horizon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(Horizon
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/ScreenItem.cpp
    src/ScreenItem.h
    src/XrandrManager.cpp
    src/XrandrManager.h
)

target_link_libraries(Horizon PRIVATE Qt6::Widgets)
