cmake_minimum_required(VERSION 3.10)
project(asio_app)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

# Server target
add_executable(server server/main.cpp)
find_path(LEVELDB_INCLUDE_DIR NAMES leveldb/db.h)
find_library(LEVELDB_LIBRARY NAMES leveldb)
if (LEVELDB_INCLUDE_DIR AND LEVELDB_LIBRARY)
    target_include_directories(server PRIVATE ${LEVELDB_INCLUDE_DIR})
    target_link_libraries(server PRIVATE ${LEVELDB_LIBRARY} Threads::Threads)
else()
    message(FATAL_ERROR "LevelDB not found!")
endif()

# Client target
add_executable(client client/main.cpp)
# Asio is header-only, so no special linking needed for client
