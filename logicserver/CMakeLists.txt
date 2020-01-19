aux_source_directory(. LOGIC_SOURCES)
file(GLOB LOGIC_HEADERS *.h *.hpp)

include_directories(${CMAKE_SOURCE_DIR}/rpc/)
include_directories(${CMAKE_SOURCE_DIR}/common/)
include_directories(${CMAKE_SOURCE_DIR}/net/)
include_directories(${CMAKE_SOURCE_DIR}/protocol/)
include_directories(${CMAKE_SOURCE_DIR}/http/)
include_directories(${CMAKE_SOURCE_DIR}/redis/)

add_executable(logicserver ${LOGIC_SOURCES} ${LOGIC_HEADERS})

target_link_libraries(logicserver
net
common
rpc
protocol
protobuf
http
pthread
rdkafka++
redis
hiredis
)