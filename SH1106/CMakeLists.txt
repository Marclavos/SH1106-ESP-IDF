# Required CMake version
cmake_minimum_required(VERSION 3.16)

# Set example as main
set(EXTRA_COMPONENT_DIRS "example")

# ESP-IDF build environment
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Project name
project(SH1106)
