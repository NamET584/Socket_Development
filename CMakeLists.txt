cmake_minimum_required(VERSION 3.15.7)
project(socket_dev)

set(CMAKE_SYSTEM_NAME Linux)

include_directories(
    ${PROJECT_SOURCE_DIR}
)

add_executable(server ${PROJECT_SOURCE_DIR}/server.cc)
add_executable(client ${PROJECT_SOURCE_DIR}/client.cc)

target_link_libraries(server PRIVATE pthread)
target_link_libraries(client PRIVATE pthread)

target_include_directories(server PRIVATE ${PROJECT_SOURCE_DIR})
target_include_directories(client PRIVATE ${PROJECT_SOURCE_DIR})

install(TARGETS server client DESTINATION ./)