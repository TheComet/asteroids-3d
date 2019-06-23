include (UrhoCommon)

if (WIN32 OR CYGWIN)
    set (PLATFORM_SOURCES
        "src/platform/not-implemented/signals.c")
elseif (UNIX)
    set (PLATFORM_SOURCES
        "src/platform/linux/signals.c")
else ()
    set (PLATFORM_SOURCES
        "src/platform/not-implemented/signals.c")
endif ()

set (TARGET_NAME asteroids-server)
set (LIBS asteroids)
set (INCLUDE_DIRS
    "include"
    "../Asteroids/include"
    "${CMAKE_CURRENT_BINARY_DIR}/../Asteroids/include/generated")
define_source_files (
    EXTRA_CPP_FILES
        "src/ServerApplication.cpp"
        "src/SignalHandler.cpp"
        "src/main.cpp"
        ${PLATFORM_SOURCES}
    GLOB_H_PATTERNS
        "include/Server/*.hpp")
setup_main_executable ()
set_output_directories (${CMAKE_RUNTIME_OUTPUT_DIRECTORY} LOCAL RUNTIME PDB)
