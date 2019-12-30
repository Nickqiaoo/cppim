cmake_minimum_required(VERSION 3.0.2)
project(chatservice)

option(FORMAT "Set to ON to format automatically." ON)
if(NOT FORMAT)
  message(STATUS "Format auto [off].")
endif()

option(BUILD_AI_CHECK "Set to ON to build ai check." OFF)
if(BUILD_AI_CHECK)
  message(STATUS "AI check [on].")
  add_definitions(-DAI_CHECK_ON)
endif()

message(STATUS "CMake version: " ${CMAKE_VERSION})

#定义 __FILENAME__ 宏，相对路径，仅仅在 GCC 编译器 上有效
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_CURRENT_SOURCE_DIR}/,,$(abspath $<))\"'")

#禁止 C++ assert terminate 程序
add_definitions(-DNDEBUG)

#ASIO 不使用 Boost 库
add_definitions(-DASIO_STANDALONE)

#Spdlog 使用外部 Fmt 库
add_definitions(-DSPDLOG_FMT_EXTERNAL)

#Jwt - cpp 使用外部 Json 库
ADD_DEFINITIONS(-DJWT_CPP_JSON_EXTERNAL)

add_definitions(-DGLOG_ON)

#生成编译命令文件，给 YCM 使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

ADD_COMPILE_OPTIONS(-g -std=c++14 -O2 -Wcomment -Wformat -Wmain -Wparentheses -Wswitch -Wunused -Wuninitialized -Wall)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

link_directories(/usr/lib64/mysql)

#source extensions，继承给子目录使用
set(CPP_EXTENSIONS *.h *.cpp)
message(STATUS "${PROJECT_NAME}, extensions: ${CPP_EXTENSIONS}")

set(CPP_DIRS common chatserver gateserver httpserver test riskserver)
message(STATUS "${PROJECT_NAME}, dirs: ${CPP_DIRS}")
foreach(EXTENSION ${CPP_EXTENSIONS})
  foreach(DIR ${CPP_DIRS})
    file(GLOB_RECURSE FIND_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/${EXTENSION})
    list(APPEND FORMAT_ALL_FILES ${FIND_FILES})
  endforeach()
endforeach()

#message(STATUS "${PROJECT_NAME}, format all files: ${FORMAT_ALL_FILES}")

add_custom_target(format-all
  COMMAND clang-format -style=\"{BasedOnStyle: google}\" -i ${FORMAT_ALL_FILES}
  COMMAND cpplint --verbose=0 --quiet ${FORMAT_ALL_FILES})

add_custom_target(format)

add_subdirectory(opt)
add_subdirectory(common)
add_subdirectory(chatserver)
add_subdirectory(gateserver)
add_subdirectory(httpserver)
add_subdirectory(test)
add_subdirectory(riskserver)
