cmake_minimum_required(VERSION 3.18)
project(cub VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17 -g -Wall")

set(MODULE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(REPO_ROOT "${MODULE_ROOT}/../..")

message("####mod dir=${MODULE_ROOT}")
message("####repo dir=${MODULE_ROOT}/../..")

SET(SRC 
    application.cpp
    aspect.cpp
    canlendar.cpp
    clock.cpp
    context.cpp
    data.cpp
    indicator.cpp
    kline.cpp
    order_mgmt.cpp
    proxy.cpp
    reactor_impl.cpp
    strategy.cpp
    trader.cpp
    ctp/ctp_md_proxy.cpp
    ctp/ctp_trade_proxy.cpp
    algos/ma.cpp
    gallery/break_through.cpp
)

add_library(${PROJECT_NAME}  ${SRC}) 

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${REPO_ROOT}/include
    ${REPO_ROOT}/lib
)

install (TARGETS ${PROJECT_NAME} DESTINATION lib)