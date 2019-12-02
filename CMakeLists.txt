cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 11)

set(PROJECT_NAME GrayBlur)

set(HEADERS pool.h)
set(SOURCES main.cpp pool.cpp)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)

find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5 COMPONENTS Widgets)

include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    )

target_link_libraries(${PROJECT_NAME} pthread opencv_core opencv_imgcodecs boost_program_options)
target_link_libraries(${PROJECT_NAME} ${Qt5Widgets_LIBRARIES})
