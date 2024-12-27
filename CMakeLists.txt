# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.12)

# set(CMAKE_C_FLAGS "-Os -DNDEBUG")
# set(CMAKE_CXX_FLAGS "-Os -DNDEBUG")

# Change your executable name to something creative!
set(NAME pico-boilerplate) # <-- Name your project/executable here!
set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)
# include(pimoroni_pico_import.cmake)

add_compile_options(-fdata-sections -ffunction-sections)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)


# Gooey boilerplate
project(pico-boilerplate C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)

# Initialize the SDK
pico_sdk_init()

include(libraries.cmake)
includeLibraries()


add_subdirectory(lib)

# Add your source files
add_executable(${NAME}
    src/main.cpp # <-- Add source files here!
    src/u8g2functions.c
    src/controller/Throttle.cpp
    src/controller/AdditionalButtons.cpp
    src/controller/BatteryTest.cpp
    src/controller/DccExController.cpp
    src/controller/KeypadManager.cpp
    # src/controller/main.cpp
    src/controller/OledDisplay.cpp
    src/controller/RotaryEncoder.cpp
    src/controller/Throttle.cpp
    src/controller/Throttles.cpp
    src/controller/WiFiManager.cpp
)

target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)

# Don't forget to link the libraries you need!
target_link_libraries(${NAME}
    encoder
    button
    pico-keypad
    hardware_pio
    hardware_base
    hardware_timer
    hardware_i2c
    hardware_spi
    pico_stdlib
    u8g2
    dccexprotocol
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip
    pico_cyw43_arch
    pico_lwip_nosys
    pico_lwip_arch
)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

# # Set up files for the release packages
# install(FILES
#     ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.uf2
#     ${CMAKE_CURRENT_LIST_DIR}/README.md
#     DESTINATION .
# )

# set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
# set(CPACK_GENERATOR "ZIP" "TGZ")
# include(CPack)
