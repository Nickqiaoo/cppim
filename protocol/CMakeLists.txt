aux_source_directory(. PROTOCOL_SOURCES)
file(GLOB PROTOCOL_HEADERS *.h *.hpp)

add_library(protocol STATIC ${PROTOCOL_SOURCES} ${PROTOCOL_HEADERS})
set_target_properties(protocol PROPERTIES FOLDER "protocol")