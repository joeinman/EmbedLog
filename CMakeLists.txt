cmake_minimum_required(VERSION 3.27.0)

project(EmbedLog VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

add_library(EmbedLog STATIC)

target_sources(EmbedLog PRIVATE
    "src/EmbedLog.cpp"
)

target_include_directories(EmbedLog PUBLIC
    "include"
)