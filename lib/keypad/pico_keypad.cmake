add_library(pico-keypad INTERFACE)

target_sources(pico-keypad INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/pico_keypad.cpp
)

target_include_directories(pico-keypad INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(pico-keypad INTERFACE pico_stdlib)