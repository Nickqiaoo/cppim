aux_source_directory(. REDIS_SOURCES)
file(GLOB REDIS_HEADERS *.h *.hpp)

include_directories(${CMAKE_SOURCE_DIR}/common/)

add_library(redis STATIC ${REDIS_SOURCES} ${REDIS_HEADERS})

set_target_properties(redis PROPERTIES FOLDER "redis")