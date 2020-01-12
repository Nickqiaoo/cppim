cmake_minimum_required(VERSION 3.0.2)
project(cppim)

message(STATUS "CMake version: " ${CMAKE_VERSION})

#定义 __FILENAME__ 宏，相对路径，仅仅在 GCC 编译器 上有效
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_CURRENT_SOURCE_DIR}/,,$(abspath $<))\"'")

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
#set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexceptions -Wno-unused -ffunction-sections -fdata-sections")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fexceptions -static-libstdc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fexceptions -fcheck-new -fnon-call-exceptions ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-rpath,../lib/so/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused -Wreorder -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")

#ASAN
if(OPEN_MEMORY_CHECK)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libasan")
  message(STATUS "Open memory check")
endif(OPEN_MEMORY_CHECK)

#gperftools
if(GPERFTOOLS)
  add_definitions(-DGPERFTOOLS)
  link_libraries(tcmalloc)
  link_libraries(profiler)
  message(STATUS "Open gperftools")
endif(GPERFTOOLS)

#debug
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -DDEBUG -O0 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wextra -Wpointer-arith")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wunreachable-code -Woverloaded-virtual")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wdouble-promotion  -g -ggdb -fpic -fPIC")

#release
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -DNDEBUG -Os -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wextra -Wpointer-arith")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wdouble-promotion -Woverloaded-virtual")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -fpic -fPIC")

message(STATUS "Compiler version: ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_VERSION}")

#ASIO 不使用 Boost 库
add_definitions(-DASIO_STANDALONE)

#Spdlog 使用外部 Fmt 库
#add_definitions(-DSPDLOG_FMT_EXTERNAL)

#ADD_COMPILE_OPTIONS(-g -std=c++14 -O2 -Wcomment -Wformat -Wmain -Wparentheses -Wswitch -Wunused -Wuninitialized -Wall)
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}/")

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

#link_directories(/usr/lib64/mysql)
#set(CPP_DIRS common chatserver gateserver httpserver test riskserver)
#message(STATUS "${PROJECT_NAME}, dirs: ${CPP_DIRS}")
#foreach(EXTENSION ${CPP_EXTENSIONS})
#  foreach(DIR ${CPP_DIRS})
#    file(GLOB_RECURSE FIND_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/${EXTENSION})
#    list(APPEND FORMAT_ALL_FILES ${FIND_FILES})
#  endforeach()
#endforeach()
#
#
#add_custom_target(format-all
#  COMMAND clang-format -style=\"{BasedOnStyle: google}\" -i ${FORMAT_ALL_FILES}
#  COMMAND cpplint --verbose=0 --quiet ${FORMAT_ALL_FILES})
#
#add_custom_target(format)
link_directories(${LIBRARY_OUTPUT_PATH})
link_directories(/usr/local/lib/)

include_directories(${CMAKE_SOURCE_DIR}/opt/spdlog/include/)
include_directories(${CMAKE_SOURCE_DIR}/opt/cpptoml/include/)
include_directories(${CMAKE_SOURCE_DIR}/opt/)

#add_subdirectory(opt)
add_subdirectory(client)
add_subdirectory(common)
add_subdirectory(http)
add_subdirectory(rpc)
add_subdirectory(net)
add_subdirectory(protocol)
add_subdirectory(logicserver)
add_subdirectory(gateserver)
add_subdirectory(jobserver)
