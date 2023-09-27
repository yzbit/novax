cmake_minimum_required(VERSION 3.18)
project(data VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17 -g")

set(MODULE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(REPO_ROOT "${MODULE_ROOT}/../..")

message("####mod dir=${MODULE_ROOT}")
message("####repo dir=${MODULE_ROOT}/../..")

SET(SRC 
    data.cpp
    indicator.cpp
    market.cpp
    ctp_proxy.cpp
    algo/ma.cpp
)


add_library(${PROJECT_NAME}  ${SRC}) 

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${REPO_ROOT}/include
    ${REPO_ROOT}/lib
)


target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall >)

add_executable(cub_data test.data.cpp)

target_include_directories(cub_data
    PRIVATE
    ${REPO_ROOT}/include
)

target_link_directories(cub_data
    PRIVATE
    ${REPO_ROOT}/lib/ctp
)

target_link_libraries(cub_data
    PRIVATE
    #reactor
    data
    thostmduserapi_se
)

install (TARGETS ${PROJECT_NAME} DESTINATION lib)
