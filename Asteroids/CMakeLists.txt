include (UrhoCommon)
include (TestVisibilityMacros)

set (TARGET_NAME asteroids)
set (ASTEROIDS_LIB_TYPE "SHARED")
set (INCLUDE_DIRS
    "include"
    "${CMAKE_CURRENT_BINARY_DIR}/include/generated")

test_visibility_macros (ASTEROIDS_API_EXPORT ASTEROIDS_API_IMPORT ASTEROIDS_API_LOCAL)

configure_file ("templates/Config.hpp.in" "include/generated/Asteroids/Config.hpp")

define_source_files (
    EXTRA_CPP_FILES
        "src/AsteroidsLib.cpp"
        "src/Menu/ConnectPrompt.cpp"
        "src/Menu/HostServerPrompt.cpp"
        "src/Menu/Menu.cpp"
        "src/Menu/MainMenu.cpp"
        "src/Menu/MenuScreen.cpp"
        "src/Objects/Asteroid.cpp"
        "src/Objects/MineController.cpp"
        "src/Objects/PhaserController.cpp"
        "src/Objects/SurfaceObject.cpp"
        "src/Player/OrbitingCameraController.cpp"
        "src/Player/ActionState.cpp"
        "src/Player/ClientLocalShipState.cpp"
        "src/Player/ClientRemoteShipState.cpp"
        "src/Player/DeviceInputMapper.cpp"
        "src/Player/ServerShipState.cpp"
        "src/Player/ShipController.cpp"
        "src/Player/WeaponSpawner.cpp"
        "src/UserRegistry/ClientUserRegistry.cpp"
        "src/UserRegistry/ServerUserRegistry.cpp"
        "src/UserRegistry/UserRegistry.cpp"
        "src/UserRegistry/User.cpp"
        "src/Util/DebugTextScroll.cpp"
        "src/Util/Process.cpp"
        "src/Util/UnidirectionalPipe.cpp"
    GLOB_H_PATTERNS
        "include/Asteroids/*.hpp"
        "include/Asteroids/Network/*.hpp"
        "include/Asteroids/Objects/*.hpp"
        "include/Asteroids/Player/*.hpp"
        "include/Asteroids/UserRegistry/*.hpp"
        "include/Asteroids/Util/*.hpp")
setup_library (${ASTEROIDS_LIB_TYPE})
target_compile_definitions (${TARGET_NAME} PRIVATE ASTEROIDS_BUILDING)

#install (TARGETS ${TARGET_NAME}
#    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
set_output_directories (${CMAKE_RUNTIME_OUTPUT_DIRECTORY} LOCAL RUNTIME PDB)
