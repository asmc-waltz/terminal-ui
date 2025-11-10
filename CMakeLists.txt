# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(terminal-ui C)

find_package(PkgConfig REQUIRED)

# LVGL
pkg_check_modules(LVGL REQUIRED lvgl)

# DRM / GBM / EGL / GLES2
pkg_check_modules(LIBDRM REQUIRED libdrm)
pkg_check_modules(GBM REQUIRED gbm)
pkg_check_modules(EGL REQUIRED egl)
pkg_check_modules(GLESv2 REQUIRED glesv2)

# DBus
pkg_check_modules(DBUS REQUIRED dbus-1)

# Include directories
include_directories(
    ${LVGL_INCLUDE_DIRS}
    ${LIBDRM_INCLUDE_DIRS}
    ${DBUS_INCLUDE_DIRS}
    ${GBM_INCLUDE_DIRS}
    ${EGL_INCLUDE_DIRS}
    ${GLESv2_INCLUDE_DIRS}
    include
)

# Library directories
link_directories(
    ${LVGL_LIBRARY_DIRS}
    ${LIBDRM_LIBRARY_DIRS}
    ${GBM_LIBRARY_DIRS}
    ${EGL_LIBRARY_DIRS}
    ${GLESv2_LIBRARY_DIRS}
)

# Source files
file(GLOB_RECURSE SRC_FILES "src/*.c")

# Terminal UI executable
add_executable(terminal-ui ${SRC_FILES})

# Link libraries
target_link_libraries(terminal-ui
    m
    lvgl
    ${LIBDRM_LIBRARIES}
    ${DBUS_LIBRARIES}
    ${GBM_LIBRARIES}
    ${EGL_LIBRARIES}
    ${GLESv2_LIBRARIES}
)

target_include_directories(terminal-ui PRIVATE ${LIBDRM_INCLUDE_DIRS})

# Utilities executable
file(GLOB_RECURSE UTILS_FILES "utils/*.c")
add_executable(ui-utils ${UTILS_FILES})
target_link_libraries(ui-utils m)
