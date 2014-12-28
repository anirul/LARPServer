cmake_minimum_required(VERSION 2.8)
project(LARPServer)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++1y")
find_package(Boost 1.52 COMPONENTS program_options date_time filesystem system thread)
include_directories(${Boost_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/../crow/include)

add_executable(LARPServer
    ${PROJECT_SOURCE_DIR}/sources/main.cpp
)

target_link_libraries(LARPServer
    ${Boost_LIBRARIES}
)
