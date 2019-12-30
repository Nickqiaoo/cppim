aux_source_directory(. COMMON_SOURCES)
file(GLOB COMMON_HEADERS *.h *.hpp)

add_library(common STATIC ${COMMON_SOURCES} ${COMMON_HEADERS})
set_target_properties(common PROPERTIES FOLDER "common")