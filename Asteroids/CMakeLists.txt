include (UrhoCommon)

set (TARGET_NAME asteroids)
set (INCLUDE_DIRS
    "include")

define_source_files(
    GLOB_CPP_PATTERNS
        "src/main.cpp"
        "src/AsteroidsApplication.cpp"
        "src/DebugTextScroll.cpp"
        "src/Player.cpp"
    GLOB_H_PATTERNS
        "include/Asteroids/*.hpp")
set (INCLUDE_DIRS
    "include")
setup_main_executable()
set_output_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY} LOCAL RUNTIME PDB)

