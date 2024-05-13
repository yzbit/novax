cmake_minimum_required(VERSION 3.18)
project(bull VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=gnu++17 -g")

set(MODULE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(REPO_ROOT "${MODULE_ROOT}/../..")


SET(SRC 
    main.cpp
)


add_executable(${PROJECT_NAME}  ${SRC}) 

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${REPO_ROOT}/include
    ${REPO_ROOT}/lib
)

target_link_directories( ${PROJECT_NAME} 
PRIVATE
    ${REPO_ROOT}/lib/ctp
)


target_link_libraries(${PROJECT_NAME} PRIVATE 
    novax
    pthread
    event
    atomic
    thosttraderapi_se.so
    thostmduserapi_se.so
    LinuxDataCollect.so
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall >)

install (TARGETS ${PROJECT_NAME} DESTINATION .)
