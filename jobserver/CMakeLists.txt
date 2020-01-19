aux_source_directory(. JOB_SOURCES)
file(GLOB JOB_HEADERS *.h *.hpp)

include_directories(${CMAKE_SOURCE_DIR}/rpc/)
include_directories(${CMAKE_SOURCE_DIR}/common/)
include_directories(${CMAKE_SOURCE_DIR}/net/)
include_directories(${CMAKE_SOURCE_DIR}/protocol/)

add_executable(jobserver ${JOB_SOURCES} ${JOB_HEADERS})
link_libraries(pthread)
target_link_libraries(jobserver
net
common
rpc
protocol
protobuf
rdkafka++
pthread
tcmalloc
)