cmake_minimum_required(VERSION 3.18)
project(bull VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=gnu++17 -g")

set(MODULE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(REPO_ROOT "${MODULE_ROOT}/../..")

add_executable(${PROJECT_NAME}  bull.cc) 
add_executable(dc.cli dc.cli.cc)
add_executable(dc.srv dc.srv.cc)

SET(INC 
    ${REPO_ROOT}/include
    ${REPO_ROOT}/lib
)

SET(LIB
    novax
    pthread
    event
    atomic
    thosttraderapi_se.so
    thostmduserapi_se.so
    LinuxDataCollect.so
)

SET(LINK
    ${REPO_ROOT}/lib/ctp
)

target_include_directories(${PROJECT_NAME}  PRIVATE ${INC})
target_link_directories( ${PROJECT_NAME} PRIVATE ${LINK})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB})

target_include_directories(dc.cli  PRIVATE ${INC})
target_link_directories( dc.cli PRIVATE ${LINK})
target_link_libraries(dc.cli PRIVATE ${LIB})

target_include_directories(dc.srv  PRIVATE ${INC})
target_link_directories( dc.srv PRIVATE ${LINK})
target_link_libraries(dc.srv PRIVATE ${LIB})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall >)


#----
install (TARGETS dc.cli dc.srv ${PROJECT_NAME} DESTINATION .)
