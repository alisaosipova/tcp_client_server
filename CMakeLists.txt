cmake_minimum_required(VERSION 3.19.4)
project(tcp_connection VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

add_executable(server serverRequest.cpp)
target_link_libraries(server PRIVATE tcp_server)

add_executable(client clientRequest.cpp)
target_link_libraries(client PRIVATE tcp_client)
