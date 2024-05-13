cmake_minimum_required(VERSION 3.18)
project(novax.ut VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=gnu++17 -g")


set(MODULE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(REPO_ROOT "${MODULE_ROOT}/../../..")

#message(${REPO_ROOT})

SET(SRC 
    main.cpp
    ut.series.cpp
    ut.ordermgmt.cpp
    ut.clock.cpp
    ut.kline.cpp
    ut.code.cpp
    ut.calendar.cpp
)

#ut coverage
add_compile_options(--coverage)
add_link_options(--coverage)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME}
PRIVATE
    ${REPO_ROOT}/lib/gtest/include
    ${REPO_ROOT}/include
    ${REPO_ROOT}/src
    ${REPO_ROOT}/lib
)

target_link_directories( ${PROJECT_NAME} 
PRIVATE
    ${REPO_ROOT}/lib/gtest/lib
    ${REPO_ROOT}/lib/ctp
)

target_link_libraries(${PROJECT_NAME} 
PRIVATE 
    novax
    pthread
    atomic
    gtest
    gmock
    event
    thosttraderapi_se.so
    thostmduserapi_se.so
    LinuxDataCollect.so
)

install (TARGETS ${PROJECT_NAME} DESTINATION .)