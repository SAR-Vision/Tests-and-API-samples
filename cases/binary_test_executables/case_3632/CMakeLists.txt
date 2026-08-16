# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

cmake_minimum_required(VERSION 3.5)
project(case_3632)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    case_3632.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}D)
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE
    $ENV{KAYA_VISION_POINT_INCLUDE_PATH})

target_link_directories(${PROJECT_NAME} PUBLIC $ENV{KAYA_VISION_POINT_LIB_PATH})

SET(KYFGLIB_LIB optimized KYFGLib debug KYFGLibD)

target_link_libraries(${PROJECT_NAME} PUBLIC ${KYFGLIB_LIB})

