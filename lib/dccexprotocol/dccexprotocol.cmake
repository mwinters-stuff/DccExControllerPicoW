add_library(dccexprotocol INTERFACE)

target_sources(dccexprotocol INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/DCCEXInbound.cpp
  ${CMAKE_CURRENT_LIST_DIR}/DCCEXLoco.cpp
  ${CMAKE_CURRENT_LIST_DIR}/DCCEXProtocol.cpp
  ${CMAKE_CURRENT_LIST_DIR}/DCCEXRoutes.cpp
  ${CMAKE_CURRENT_LIST_DIR}/DCCEXTurnouts.cpp
  ${CMAKE_CURRENT_LIST_DIR}/DCCEXTurntables.cpp
)

target_include_directories(dccexprotocol INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Pull in pico libraries that we need
target_link_libraries(dccexprotocol INTERFACE pico_stdlib)