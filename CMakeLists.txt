cmake_minimum_required(VERSION 3.28)

project(EmbedLog)

add_library(EmbedLog INTERFACE)

target_include_directories(EmbedLog INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
