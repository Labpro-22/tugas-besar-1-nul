cmake_minimum_required(VERSION 3.16)

project(game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(TEST_DIR "${CMAKE_SOURCE_DIR}/test")

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${SRC_DIR}/*.cpp"
)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    "${TEST_DIR}/*_test.cpp"
)

if(PROJECT_SOURCES STREQUAL "")
    message(FATAL_ERROR "No source files found in ${SRC_DIR}")
endif()

set(MAIN_SOURCE "${SRC_DIR}/main.cpp")
set(CORE_SOURCES ${PROJECT_SOURCES})
list(REMOVE_ITEM CORE_SOURCES "${MAIN_SOURCE}")

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

if(TEST_SOURCES)
    set(ALL_TEST_TARGETS "")

    foreach(TEST_FILE ${TEST_SOURCES})
        file(RELATIVE_PATH TEST_FILE_REL "${TEST_DIR}" "${TEST_FILE}")
        get_filename_component(TEST_TARGET "${TEST_FILE}" NAME_WE)

        add_executable(${TEST_TARGET}
            "${TEST_FILE}"
            ${CORE_SOURCES}
        )

        target_include_directories(${TEST_TARGET}
            PRIVATE
                "${CMAKE_SOURCE_DIR}/include"
        )

        target_compile_options(${TEST_TARGET}
            PRIVATE
                -Wall
                -Wextra
        )

        list(APPEND ALL_TEST_TARGETS ${TEST_TARGET})
        message(STATUS "Registered test target: ${TEST_TARGET} (${TEST_FILE_REL})")
    endforeach()

    add_custom_target(all_tests DEPENDS ${ALL_TEST_TARGETS})
endif()
