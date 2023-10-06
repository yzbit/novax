cmake_minimum_required(VERSION 3.18)
project(all_test VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17 -g")

set(MODULE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(REPO_ROOT "${MODULE_ROOT}/../..")
add_compile_options(-I${REPO_ROOT}/include)
#add_link_options(-lzmq -lcub)

#add_executable(test_reactor test.pub.cpp)

add_executable(test_kline test.kline.cpp ../core/aspect.cpp ../core/algos/klines.cpp ../core/indicator.cpp)

#[[
target_link_libraries(test_reactor PRIVATE 
    pthread
    cub
    zmq
)
]]