cmake_minimum_required(VERSION 3.18)
project(cub VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17 -g -Wall")

set(MODULE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(REPO_ROOT "${MODULE_ROOT}/../..")

message("####mod dir=${MODULE_ROOT}")
message("####repo dir=${MODULE_ROOT}/../..")

SET(SRC 
    reactor_impl.cpp
    application.cpp
    data.cpp
    algo.cpp
    clock.cpp
    market.cpp
    indicator.cpp
    aspect.cpp
    ctp/ctp_md_proxy.cpp
    strategy.cpp
)

add_library(${PROJECT_NAME}  ${SRC}) 

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${REPO_ROOT}/include
    ${REPO_ROOT}/lib
)

install (TARGETS ${PROJECT_NAME} DESTINATION lib)