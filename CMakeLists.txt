cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:200000000 /HEAP:200000000")

project(CROSSTONES_V4)

find_package(raylib CONFIG REQUIRED)

add_executable(crosstones_executable src/game-suite/Main.cpp)

target_link_libraries(crosstones_executable PRIVATE raylib)
set(MY_MODULES
    felix-bots
    game-suite
    globals
    test-bots
    test-suite
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

    target_link_libraries(crosstones_executable PRIVATE ${MOD})
endforeach()

