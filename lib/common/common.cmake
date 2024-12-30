add_library(common INTERFACE)

# target_sources(common INTERFACE
#   ${CMAKE_CURRENT_LIST_DIR}/common.cpp
# )

target_include_directories(common INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(common INTERFACE pico_stdlib)
