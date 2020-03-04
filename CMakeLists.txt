cmake_minimum_required(VERSION 3.0)
project(tch_cfe)

set(CMAKE_BUILD_TYPE Debug)

file(GLOB sources src/*.cpp)
add_executable(main ${sources})
target_link_directories(main PUBLIC ${CMAKE_CURRENT_LIST_DIR}/unicorn-1.0.1-win64)
target_include_directories(main PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(main unicorn)
set_property(TARGET main PROPERTY CXX_STANDARD 17)