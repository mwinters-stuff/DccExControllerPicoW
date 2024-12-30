add_library(controller INTERFACE)

file(GLOB SOURCES "*.c" "*.cpp")

target_sources(controller INTERFACE ${SOURCES})

target_include_directories(controller INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(controller INTERFACE 
  pico_stdlib
  encoder
  button
  pico-keypad
  dccexprotocol
  )