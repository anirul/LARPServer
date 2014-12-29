cmake_minimum_required(VERSION 3.0)
project(LARPServer)

include("cmake/FindCXXFeatures.cmake")
include("cmake/FindSQLite3.cmake")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++1y")

find_package(Boost 1.52 REQUIRED COMPONENTS
    program_options
    date_time
    filesystem
    system
    thread
)
include_directories(${Boost_INCLUDE_DIR})
include_directories(${SQLITE3_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/crow)

add_executable(LARPServer
    ${PROJECT_SOURCE_DIR}/sources/main.cpp
    ${PROJECT_SOURCE_DIR}/sources/lite_db.hpp
    ${PROJECT_SOURCE_DIR}/sources/lite_db.cpp
)

target_link_libraries(LARPServer
    ${Boost_LIBRARIES}
    ${SQLITE3_LIBRARIES}
)
