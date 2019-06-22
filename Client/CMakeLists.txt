include (UrhoCommon)

set (TARGET_NAME asteroids-client)
set (LIBS asteroids)
set (INCLUDE_DIRS
    "include"
    "../Asteroids/include"
    "${CMAKE_CURRENT_BINARY_DIR}/../Asteroids/include/generated")
define_source_files (
    EXTRA_CPP_FILES
        "src/ClientApplication.cpp"
        "src/main.cpp"
    GLOB_H_PATTERNS
        "include/Client/*.hpp")
setup_main_executable ()
set_output_directories (${CMAKE_RUNTIME_OUTPUT_DIRECTORY} LOCAL RUNTIME PDB)
