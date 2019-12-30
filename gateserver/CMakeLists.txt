aux_source_directory(. GATE_SOURCES)
file(GLOB GATE_HEADERS *.h *.hpp)

include_directories(${CMAKE_SOURCE_DIR}/rpc/)
include_directories(${CMAKE_SOURCE_DIR}/common/)
include_directories(${CMAKE_SOURCE_DIR}/net/)
include_directories(${CMAKE_SOURCE_DIR}/protocol/)

add_executable(gateserver ${GATE_SOURCES} ${GATE_HEADERS})
target_link_libraries(gateserver
net
common
rpc
protocol
protobuf
)