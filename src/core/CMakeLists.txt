cmake_minimum_required(VERSION 3.18)
project(novax VERSION 1.0.0 LANGUAGES CXX)


#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17 -g -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17 -g -Wall")

set(MODULE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(REPO_ROOT "${MODULE_ROOT}/../..")

SET(SRC 
    aspect.cpp
    calendar.cpp
    clock.cpp
    context.cpp
    #datacenter0.cpp
    #datacenter1.cpp
    data.cpp
    kline.cpp
    ordermgmt.cpp
    ctp/mdproxy.cpp
    ctp/tradeproxy.cpp
    proxy.cpp
    quant.cpp
    #dbagent.cpp
    config.cpp
    #algos/ma.cpp
)

add_library(${PROJECT_NAME}  ${SRC}) 

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${REPO_ROOT}/include
    ${REPO_ROOT}/lib
    ${REPO_ROOT}/3rd
)

SET(CFGS
    ${REPO_ROOT}/src/core/ctp/md.json 
    ${REPO_ROOT}/src/core/ctp/cal.json 
    ${REPO_ROOT}/src/core/ctp/trade.json 
)

SET(DEPS
    ${REPO_ROOT}/lib/ctp/libLinuxDataCollect.so
    ${REPO_ROOT}/lib/ctp/libthostmduserapi_se.so
    ${REPO_ROOT}/lib/ctp/libthosttraderapi_se.so
)

install (TARGETS ${PROJECT_NAME} DESTINATION lib)
install (FILES  ${CFGS} DESTINATION conf.d/ctp)
install (FILES  ${DEPS} DESTINATION .)