cmake_minimum_required(VERSION 3.10)
project(server)

set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/bin)    # File thực thi
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/bin)    # Thư viện động (.so, .dll)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}/bin)    # Thư viện tĩnh (.a, .lib)

set(CMAKE_CXX_STANDARD 17)
file(GLOB SERVER_SOURCES src/*.cpp)

find_package(Boost REQUIRED COMPONENTS system thread)
find_package(nlohmann_json REQUIRED)

add_executable(server ${SERVER_SOURCES})

target_include_directories(server PUBLIC include)
if(WIN32) 
	target_link_libraries(server PUBLIC ${Boost_LIBRARIES} ws2_32 wsock32 nlohmann_json::nlohmann_json)
else()
	target_link_libraries(server PUBLIC ${Boost_LIBRARIES} nlohmann_json::nlohmann_json)