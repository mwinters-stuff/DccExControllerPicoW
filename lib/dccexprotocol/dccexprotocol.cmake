add_library(dccexprotocol INTERFACE)

file(GLOB SOURCES "*.c" "*.cpp")

target_sources(dccexprotocol INTERFACE ${SOURCES})

target_include_directories(dccexprotocol INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(dccexprotocol INTERFACE pico_stdlib)