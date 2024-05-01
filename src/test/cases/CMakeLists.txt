cmake_minimum_required(VERSION 3.18)
project(novax.ut VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=gnu++17 -g")

set(MODULE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(REPO_ROOT "${MODULE_ROOT}/../../..")

#message(${REPO_ROOT})

SET(SRC 
    main.cpp
    ut.series.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME}
PRIVATE
    ${REPO_ROOT}/lib/gtest/include
    ${REPO_ROOT}/include
)

target_link_directories( ${PROJECT_NAME} 
PRIVATE
    ${REPO_ROOT}/lib/gtest/lib
)

target_link_libraries(${PROJECT_NAME} 
PRIVATE 
    pthread
    event
    atomic
    gtest
    gmock
)