# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.22)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C++ standards
project(four_seven_segment_display C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.1")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.1 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME}
    main.c
)

target_compile_options(${PROJECT_NAME}
  PRIVATE $<$<COMPILE_LANGUAGE:C>:-Wall -Wextra -Wendif-labels -Wformat=2 -Winit-self -Wswitch-enum -Wshadow -Wcast-align -Wwrite-strings -Wjump-misses-init -Wlogical-op -Wstrict-prototypes -Wredundant-decls -Wformat-security>
)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
)

# Disable usb output, enable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
