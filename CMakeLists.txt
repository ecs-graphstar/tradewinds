cmake_minimum_required(VERSION 3.18)
project(tradewinds)

# Tradewinds is built as a SHARED library and integrated into a parent project.
# The parent project is expected to provide the `flecs` target; tradewinds only
# owns its nested libzmq + msgpack-cxx dependencies.

# msgpack-cxx (header-only INTERFACE target)
add_subdirectory("deps/msgpack-c")

# libzmq (shared)
set(WITH_DOC OFF CACHE BOOL "" FORCE)
set(ZMQ_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED ON CACHE BOOL "" FORCE)
set(BUILD_STATIC OFF CACHE BOOL "" FORCE)
add_subdirectory("deps/libzmq")

add_library(tradewinds SHARED src/tradewinds.cpp)
set_property(TARGET tradewinds PROPERTY CXX_STANDARD 17)

target_include_directories(tradewinds PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/cppzmq"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/libzmq/include"
)

target_link_libraries(tradewinds PUBLIC flecs libzmq msgpack-cxx)
