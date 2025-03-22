# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(terminal-ui)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LVGL REQUIRED lvgl)
pkg_check_modules(LIBDRM REQUIRED libdrm)

include_directories(${LVGL_INCLUDE_DIRS})
link_directories(${LVGL_LIBRARY_DIRS})

add_executable(terminal-ui terminal-ui.c)

target_link_libraries(terminal-ui lvgl)
target_link_libraries(terminal-ui ${LIBDRM_LIBRARIES})
target_include_directories(terminal-ui PRIVATE ${LIBDRM_INCLUDE_DIRS})


