include (UrhoCommon)
include (TestVisibilityMacros)

set (TARGET_NAME asteroids)
set (ASTEROIDS_LIB_TYPE "SHARED")
set (INCLUDE_DIRS
    "include"
    "${CMAKE_CURRENT_BINARY_DIR}/include/generated")

test_visibility_macros (ASTEROIDS_API_EXPORT ASTEROIDS_API_IMPORT ASTEROIDS_API_LOCAL)

configure_file ("templates/Config.hpp.in" "include/generated/Asteroids/Config.hpp")

define_source_files(
    EXTRA_CPP_FILES
        "src/ActionState.cpp"
        "src/Asteroid.cpp"
        "src/AsteroidsLib.cpp"
        "src/Bullet.cpp"
        "src/DebugTextScroll.cpp"
        "src/DeviceInputMapper.cpp"
        "src/OrbitingCameraController.cpp"
        "src/ShipController.cpp"
        "src/SurfaceObject.cpp"
    GLOB_H_PATTERNS
        "include/Asteroids/*.hpp")
setup_library (${ASTEROIDS_LIB_TYPE})
target_compile_definitions (${TARGET_NAME} PRIVATE ASTEROIDS_BUILDING)

install (TARGETS ${TARGET_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
set_output_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY} LOCAL RUNTIME PDB)
