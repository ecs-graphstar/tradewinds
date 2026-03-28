cmake_minimum_required(VERSION 3.18)
project(project)

add_executable(${PROJECT_NAME} src/main.cpp src/tradewinds.cpp)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_include_directories(${PROJECT_NAME} PRIVATE "src")

target_include_directories(${PROJECT_NAME} PRIVATE "deps/cppzmq")

# flecs
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build Flecs as a shared library" FORCE)
add_subdirectory("deps/flecs")
target_link_libraries(${PROJECT_NAME} PRIVATE flecs)

include_directories(deps/msgpack-c/include)
add_subdirectory("deps/msgpack-c")
target_link_libraries(${PROJECT_NAME} PRIVATE msgpack-cxx)

# build libzmq from source
set(WITH_DOC OFF CACHE BOOL "" FORCE)
set(ZMQ_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED ON CACHE BOOL "" FORCE)
set(BUILD_STATIC OFF CACHE BOOL "" FORCE)
add_subdirectory("deps/libzmq")

target_link_libraries(${PROJECT_NAME} PRIVATE libzmq)
target_include_directories(${PROJECT_NAME} PRIVATE "deps/libzmq/include")
