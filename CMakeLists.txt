cmake_minimum_required(VERSION 3.16)

project(game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${SRC_DIR}/*.cpp"
)

if(PROJECT_SOURCES STREQUAL "")
    message(FATAL_ERROR "No source files found in ${SRC_DIR}")
endif()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
        -Wall
        -Wextra
)
