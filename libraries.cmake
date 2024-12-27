cmake_minimum_required(VERSION 3.16)
function (includeLibraries)

include(FetchContent)

FetchContent_Declare(
  u8g2
  GIT_REPOSITORY    https://github.com/olikraus/u8g2.git
  # GIT_REPOSITORY    git@github.com:olikraus/u8g2.git
  GIT_TAG           master
  GIT_SHALLOW       1
)
# FetchContent_GetProperties(u8g2)

# if(NOT u8g2_POPULATED)
#   FetchContent_Populate(u8g2)
#   # execute_process(WORKING_DIRECTORY ${u8g2_SOURCE_DIR} COMMAND bash -c ${CMAKE_SOURCE_DIR}/patch-u8g2.sh )
#   add_subdirectory(${u8g2_SOURCE_DIR} ${u8g2_BINARY_DIR})
# endif()

FetchContent_MakeAvailable(u8g2)


# FetchContent_Declare(
#   DCCEXProtocol
#   GIT_REPOSITORY    https://github.com/DCC-EX/DCCEXProtocol.git
#   # GIT_REPOSITORY      git@github.com:DCC-EX/DCCEXProtocol.git
#   GIT_TAG           main
#   GIT_SHALLOW       1
# )
# FetchContent_GetProperties(DCCEXProtocol)

# if(NOT dccexprotocol_POPULATED)
#   FetchContent_Populate(DCCEXProtocol)

#   # message(STATUS "Showing Properties")
#   # get_cmake_property(properties VARIABLES)
#   # foreach(property ${properties})
#   #     # if(property MATCHES "^DCC")
#   #         message(STATUS "${property} = ${${property}}")
#   #     # endif()
#   # endforeach()

#   execute_process(
#       WORKING_DIRECTORY ${dccexprotocol_SOURCE_DIR} 
#       COMMAND ${CMAKE_SOURCE_DIR}/patch-dccexprotocol.sh 
#   )

# endif()

# FetchContent_MakeAvailable(DCCEXProtocol)


endfunction()
