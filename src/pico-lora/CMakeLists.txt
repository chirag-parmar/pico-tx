cmake_minimum_required(VERSION 3.12)

add_library(pico-lora lora.c lora.h)

target_include_directories(pico-lora PUBLIC
        .
)

target_link_libraries(pico-lora pico_stdlib hardware_spi hardware_interp LoRa_print)