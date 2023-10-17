cmake_minimum_required(VERSION 3.18)
project(all_test VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17 -g")
set(LIBPATH ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)

message("libpath=" ${LIBPATH})

include_directories(${LIBPATH})
link_directories(${LIBPATH}/ctp)

#add_executable(test_rb test.ringbuff.cpp)
#add_executable(test_asp test.aspect.kline.cpp  ../core/indicator.cpp ../core/aspect.cpp ../core/kline.cpp ../core/clock.cpp )
#add_executable(test_data test.data.cpp ../core/data.cpp ../core/ctp/ctp_md_proxy.cpp  ../core/aspect.cpp ../core/clock.cpp ../core/indicator.cpp ../core/aspect.cpp ../core/kline.cpp)

add_executable(test_om test.omgmt.cpp ../core/order_mgmt.cpp ../core/ctp/ctp_trade_proxy.cpp)

#[[
target_link_libraries(test_data
PRIVATE
thostmduserapi_se.so
)
]]

target_link_libraries(test_om
PRIVATE
libthosttraderapi_se.so
)

#add_executable(test_mgmt test.omgmt.cpp ../core/order_mgmt.cpp)

#add_executable(test_reactor test.pub.cpp)
#add_executable(test_kline test.kline.cpp ../core/aspect.cpp ../core/algos/klines.cpp ../core/indicator.cpp)
#[[
target_link_libraries(test_reactor PRIVATE 
    pthread
    cub
    zmq
)
]]

#[[
function(redefine_file_macro targetname)
    #获取当前目标的所有源文件
    get_target_property(source_files "${targetname}" SOURCES)
    
    #file(GLOB_RECURSE source_files "*.cpp" "*.c" "/home/data/code/cub/*.h")	

    #遍历源文件
    foreach(sourcefile ${source_files})
        #获取当前源文件的编译参数
        get_property(defs SOURCE "${sourcefile}"
            PROPERTY COMPILE_DEFINITIONS)
        #获取当前文件的绝对路径
        get_filename_component(filepath "${sourcefile}" ABSOLUTE)
        #将绝对路径中的项目路径替换成空,得到源文件相对于项目路径的相对路径
        string(REPLACE ${PROJECT_SOURCE_DIR}/ "" relpath ${filepath})
        #将我们要加的编译参数(__FILE__定义)添加到原来的编译参数里面
        list(APPEND defs "__FILE__=\"${relpath}\"")
        #重新设置源文件的编译参数
        set_property(
            SOURCE "${sourcefile}"
            PROPERTY COMPILE_DEFINITIONS ${defs}
            )
    endforeach()

add_definitions(-Wno-builtin-macro-redefined)
endfunction()

function(add_srcs NAME)
  set(${NAME} ${ARGN} PARENT_SCOPE)
  foreach(f IN LISTS ARGN)
    file(RELATIVE_PATH b ${CMAKE_SOURCE_DIR}
         ${CMAKE_CURRENT_SOURCE_DIR}/${f})
    set_source_files_properties(${f} PROPERTIES COMPILE_DEFINITIONS
                                "__FILE__=\"${b}\"")
  endforeach()
endfunction()

set(MODULE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(REPO_ROOT "${MODULE_ROOT}/../..")
add_compile_options(-I${REPO_ROOT}/include)

message(Repo=${REPO_ROOT})
#add_srcs(RBSRC test.ringbuff.cpp ../core/ring_buffer.h)
#redefine_file_macro(test_rb)
]]
