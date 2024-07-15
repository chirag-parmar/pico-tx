cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(pico-sdk/pico_sdk_init.cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

project(pico-tx)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

if (TARGET tinyusb_device)

    add_subdirectory(src/pico-lora)
    
    add_executable(pico-tx
        src/main.cpp
    )

    target_include_directories(pico-tx PUBLIC src/)

    target_link_libraries(pico-tx pico_stdlib LoRa_pico_lib)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(pico-tx 1)
    pico_enable_stdio_uart(pico-tx 0)

    # create map/bin/hex/uf2 file in addition to ELF.
    pico_add_extra_outputs(pico-tx)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()