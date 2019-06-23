cmake_minimum_required (VERSION 3.3)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules")

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Debug CACHE STRING "")
endif ()

project ("Asteroids 3D")

add_subdirectory ("Asteroids")
add_subdirectory ("Client")
add_subdirectory ("Server")
add_subdirectory ("Editor")
