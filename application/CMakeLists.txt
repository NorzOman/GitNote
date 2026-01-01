cmake_minimum_required(VERSION 3.16)

project(Application LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(Application
    gitnote.cpp

    editorHandler/editorHandler.cpp
    editorHandler/editorHandler.h

    loginHandler/loginHandler.cpp
    loginHandler/loginHandler.h

    authHandler/authHandler.cpp
    authHandler/authHandler.h

    dashboardHandler/dashboardHandler.h
    dashboardHandler/dashboardHandler.cpp
)

target_link_libraries(Application
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
)
