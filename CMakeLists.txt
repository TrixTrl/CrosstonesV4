cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

project(CROSSTONES_V4)

find_package(raylib CONFIG REQUIRED)

# Font download (once — subsequent builds use cached file)
set(FONT_PATH "${CMAKE_SOURCE_DIR}/resources/FiraSans-Regular.ttf")
if(NOT EXISTS ${FONT_PATH})
    message(STATUS "Downloading FiraSans font ...")
    file(DOWNLOAD "https://raw.githubusercontent.com/mozilla/Fira/master/ttf/FiraSans-Regular.ttf"
                  ${FONT_PATH} TLS_VERIFY OFF INACTIVITY_TIMEOUT 30)
endif()
# If CMake download failed, try PowerShell
if(NOT EXISTS ${FONT_PATH})
    message(STATUS "Trying PowerShell download ...")
    execute_process(COMMAND powershell -Command
        "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/mozilla/Fira/master/ttf/FiraSans-Regular.ttf' -OutFile '${FONT_PATH}' -UseBasicParsing"
        ERROR_QUIET OUTPUT_QUIET)
endif()
if(NOT EXISTS ${FONT_PATH})
    message(WARNING "Font download failed — will use built-in fallback font")
else()
    file(SIZE ${FONT_PATH} FONT_SIZE)
    if(FONT_SIZE EQUAL 0)
        file(REMOVE ${FONT_PATH})
        message(WARNING "Font file is empty — will use built-in fallback font")
    else()
        message(STATUS "Font ready (${FONT_SIZE} bytes)")
    endif()
endif()

add_executable(crosstones_executable src/Main.cpp)

target_include_directories(crosstones_executable PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(crosstones_executable PRIVATE raylib)
target_compile_options(crosstones_executable PRIVATE /EHsc)

# Copy resources to build output directory
add_custom_command(TARGET crosstones_executable POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:crosstones_executable>/resources)

set(MY_MODULES
    app
    felix-bots
    game-suite
    globals
    test-bots
    trix-bots
)

foreach(MOD ${MY_MODULES})
    message(STATUS "Configuring module: ${MOD}")

    file(GLOB_RECURSE MOD_SOURCES CONFIGURE_DEPENDS 
        "${CMAKE_CURRENT_SOURCE_DIR}/src/${MOD}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/${MOD}/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/${MOD}/*.hpp"
    )

    add_library(${MOD} STATIC ${MOD_SOURCES})

    target_include_directories(${MOD} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
    
    target_link_libraries(${MOD} PUBLIC raylib)
    target_compile_options(${MOD} PUBLIC /EHsc)

    target_link_libraries(crosstones_executable PRIVATE ${MOD})
endforeach()

# Keep the test suite as a standalone console executable
file(GLOB_RECURSE TEST_SUITE_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/test-suite/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/test-suite/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/test-suite/*.hpp"
)
add_executable(crosstones_tests ${TEST_SUITE_SOURCES})
target_include_directories(crosstones_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(crosstones_tests PRIVATE game-suite globals)
target_compile_options(crosstones_tests PRIVATE /EHsc)
