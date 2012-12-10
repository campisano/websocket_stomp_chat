cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-Wall -pipe")

set(var_target websocket_stomp_chat)
set(var_path_source src)
project(Prj_${var_target})

include_directories(/usr/local/libwebsockets/include)
link_directories(/usr/local/libwebsockets/lib)

file(GLOB_RECURSE var_sources ${var_path_source}/*.cpp)
add_executable(${var_target} ${var_sources})
target_link_libraries(${var_target} websockets)

install(TARGETS ${var_target} DESTINATION ${PROJECT_SOURCE_DIR}/install)
