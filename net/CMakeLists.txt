aux_source_directory(. NET_SOURCES)
file(GLOB NET_HEADERS *.h *.hpp)

include_directories(${CMAKE_SOURCE_DIR}/rpc/)
include_directories(${CMAKE_SOURCE_DIR}/common/)
include_directories(${CMAKE_SOURCE_DIR}/net/)
include_directories(${CMAKE_SOURCE_DIR}/http/)

add_library(net STATIC ${NET_SOURCES} ${NET_HEADERS})
set_target_properties(net PROPERTIES FOLDER "net")