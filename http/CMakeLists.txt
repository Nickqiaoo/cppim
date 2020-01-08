aux_source_directory(. HTTP_SOURCES)
file(GLOB HTTP_HEADERS *.h *.hpp)

include_directories(${CMAKE_SOURCE_DIR}/common/)
include_directories(${CMAKE_SOURCE_DIR}/net/)

add_library(http STATIC ${HTTP_SOURCES} ${HTTP_HEADERS})
set_target_properties(http PROPERTIES FOLDER "http")