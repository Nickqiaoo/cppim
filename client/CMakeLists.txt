
include_directories(${CMAKE_SOURCE_DIR}/rpc/)
include_directories(${CMAKE_SOURCE_DIR}/common/)
include_directories(${CMAKE_SOURCE_DIR}/net/)
include_directories(${CMAKE_SOURCE_DIR}/protocol/)
include_directories(${CMAKE_SOURCE_DIR}/gateserver/)

add_executable(tcpclient tcpclient.cc)
target_link_libraries(tcpclient
net
common
rpc
protocol
protobuf
pthread
)