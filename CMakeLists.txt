cmake_minimum_required(VERSION 3.0.2)
project(cppim)

message(STATUS "CMake version: " ${CMAKE_VERSION})

#定义 __FILENAME__ 宏，相对路径，仅仅在 GCC 编译器 上有效
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_CURRENT_SOURCE_DIR}/,,$(abspath $<))\"'")

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexceptions -Wno-unused -ffunction-sections -fdata-sections")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fexceptions -static-libstdc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std-c++17 -fexceptions -fcheck-new -fnon-call-exceptions ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-rpath,../lib/so/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused -Wreorder -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-amit-frame-pointer")

#ASAN
if(OPEN_MEMORY_CHECK)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libasan")
  message(STATUS "Open memory check")
endif(OPEN_MEMORY_CHECK)

#debug

#release


#ASIO 不使用 Boost 库
add_definitions(-DASIO_STANDALONE)

#Spdlog 使用外部 Fmt 库
add_definitions(-DSPDLOG_FMT_EXTERNAL)

ADD_COMPILE_OPTIONS(-g -std=c++14 -O2 -Wcomment -Wformat -Wmain -Wparentheses -Wswitch -Wunused -Wuninitialized -Wall)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

link_directories(/usr/lib64/mysql)

set(CPP_DIRS common chatserver gateserver httpserver test riskserver)
message(STATUS "${PROJECT_NAME}, dirs: ${CPP_DIRS}")
foreach(EXTENSION ${CPP_EXTENSIONS})
  foreach(DIR ${CPP_DIRS})
    file(GLOB_RECURSE FIND_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/${EXTENSION})
    list(APPEND FORMAT_ALL_FILES ${FIND_FILES})
  endforeach()
endforeach()


add_custom_target(format-all
  COMMAND clang-format -style=\"{BasedOnStyle: google}\" -i ${FORMAT_ALL_FILES}
  COMMAND cpplint --verbose=0 --quiet ${FORMAT_ALL_FILES})

add_custom_target(format)

add_subdirectory(opt)
add_subdirectory(common)
add_subdirectory(rpc)
add_subdirectory(net)
add_subdirectory(logicserver)
add_subdirectory(gateserver)
add_subdirectory(jobserver)
