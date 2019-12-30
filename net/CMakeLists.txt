aux_source_directory(. NET_SOURCES)
file(GLOB NET_HEADERS *.h *.hpp)

add_library(net STATIC ${NET_SOURCES} ${NET_HEADERS})
set_target_properties(net PROPERTIES FOLDER "net")