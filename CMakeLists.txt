project(asio) 

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

#add_definitions(-DASIO_STANDALONE)
add_executable(httpserver server.cc HttpServer.cc HttpContext.cc HttpSession.cc )
#target_link_libraries(httpserver libhello.a)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")