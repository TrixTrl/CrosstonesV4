cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:200000000 /HEAP:200000000")

project(CROSSTONES_V4)

find_package(raylib CONFIG REQUIRED)

# Font download (once — subsequent builds use cached file)
set(FONT_PATH "${CMAKE_SOURCE_DIR}/resources/DejaVuSans.ttf")
if(NOT EXISTS ${FONT_PATH})
    message(STATUS "Downloading DejaVu Sans font ...")
    file(DOWNLOAD "https://raw.githubusercontent.com/prawnpdf/prawn/master/data/fonts/DejaVuSans.ttf"
                  ${FONT_PATH} TLS_VERIFY OFF INACTIVITY_TIMEOUT 30)
endif()
# If CMake download failed, try PowerShell
if(NOT EXISTS ${FONT_PATH})
    message(STATUS "Trying PowerShell download ...")
    execute_process(COMMAND powershell -Command
        "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/prawnpdf/prawn/master/data/fonts/DejaVuSans.ttf' -OutFile '${FONT_PATH}' -UseBasicParsing"
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
file(GLOB_RECURSE RESOURCE_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/resources/*")

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/resources.stamp
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:crosstones_executable>/resources
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/resources.stamp
    DEPENDS ${RESOURCE_FILES}
    COMMENT "Copying resources"
)
add_custom_target(copy_resources ALL DEPENDS ${CMAKE_BINARY_DIR}/resources.stamp)
add_dependencies(crosstones_executable copy_resources)


set(MY_MODULES
    app
    data
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
    file(GLOB_RECURSE MOD_CPP_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/${MOD}/*.cpp"
    )

    # Header-only modules (no .cpp) need an INTERFACE library — CMake can't
    # pick a linker language for a STATIC library with nothing to compile.
    if(MOD_CPP_SOURCES)
        set(MOD_SCOPE PUBLIC)
        add_library(${MOD} STATIC ${MOD_SOURCES})
    else()
        set(MOD_SCOPE INTERFACE)
        add_library(${MOD} INTERFACE)
        target_sources(${MOD} INTERFACE ${MOD_SOURCES})
    endif()

    target_include_directories(${MOD} ${MOD_SCOPE} ${CMAKE_CURRENT_SOURCE_DIR}/src)

    target_link_libraries(${MOD} ${MOD_SCOPE} raylib)
    target_compile_options(${MOD} ${MOD_SCOPE} /EHsc)

    target_link_libraries(crosstones_executable PRIVATE ${MOD})
endforeach()

option(BUILD_TESTS "Build self-tests" ON)
if(BUILD_TESTS)
    enable_testing()
    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/tests/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/tests/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/tests/*.hpp"
    )
    target_sources(crosstones_executable PRIVATE ${TEST_SOURCES})
    # auto discover tests with *Tester.cpp suffix
    file(GLOB TEST_CTEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/tests/*Tester.cpp"
    )
    # Tests that are slow/verbose enough to be excluded by default
    # To run, use:
    # 'ctest -R <name>' or 'crosstones_executable --run-tests <name>'
    set(MANUAL_TESTS
        "OpeningsVerifyTester"
    )
    foreach(TSF ${TEST_CTEST_SOURCES})
        get_filename_component(TEST_NAME ${TSF} NAME_WE)
        add_test(NAME ${TEST_NAME}
            COMMAND crosstones_executable --run-tests ${TEST_NAME})
        if(TEST_NAME IN_LIST MANUAL_TESTS)
            set_tests_properties(${TEST_NAME} PROPERTIES LABELS "manual")
        endif()
    endforeach()
    # Run all non-manual tests on every build. Failures are printed to the console
    add_custom_target(check ALL
        COMMAND cmd /C "\"${CMAKE_CTEST_COMMAND}\" --output-on-failure -LE manual & exit 0"
        DEPENDS crosstones_executable
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        USES_TERMINAL
    )
endif()
