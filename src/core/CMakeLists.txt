cmake_minimum_required(VERSION 3.18)
project(novax VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17 -g -Wall")

set(MODULE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(REPO_ROOT "${MODULE_ROOT}/../..")

SET(SRC 
    aspect.cpp
    calendar.cpp
    clock.cpp
    context.cpp
    data_center.cpp
    data.cpp
    kline.cpp
    order_mgmt.cpp
    ctp/ctp_md_proxy.cpp
    ctp/ctp_trade_proxy.cpp
    proxy.cpp
    quant.cpp
    dbagent.cpp
    #algos/ma.cpp
)

add_library(${PROJECT_NAME}  ${SRC}) 

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${REPO_ROOT}/include
    ${REPO_ROOT}/lib
)


SET(CFGS
    ${REPO_ROOT}/src/core/ctp/md.json 
    ${REPO_ROOT}/src/core/ctp/cal.json 
    ${REPO_ROOT}/src/core/ctp/trade.json 
)

install (TARGETS ${PROJECT_NAME} DESTINATION lib)
install (FILES  ${CFGS} DESTINATION conf.d/ctp)