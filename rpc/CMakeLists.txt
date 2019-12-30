aux_source_directory(. RPC_SOURCES)
file(GLOB RPC_HEADERS *.h *.hpp)

include_directories(${CMAKE_SOURCE_DIR}/net/)
include_directories(${CMAKE_SOURCE_DIR}/common/)

add_library(rpc STATIC ${RPC_SOURCES} ${RPC_HEADERS})
set_target_properties(rpc PROPERTIES FOLDER "rpc")