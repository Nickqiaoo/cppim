aux_source_directory(. GATE_SOURCES)
file(GLOB GATE_HEADERS *.h *.hpp)

add_executable(gateserver ${GATE_SOURCES} ${GATE_HEADERS})
target_link_libraries(gateserver
net
common
rpc
protobuf
)