cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 20)
project(6502Emu VERSION 0.1.0)

include(CTest)
enable_testing()

file(GLOB_RECURSE SOURCES src/*.cpp)

add_definitions("-DRESOURCE_PATH=\"${PROJECT_SOURCE_DIR}/resources\"")
add_definitions("-DDISSASMBLY_LOG_PATH=\"${PROJECT_SOURCE_DIR}/dissasmbly/log\"")

#add_definitions("-DNESTEST_DEBUG")

include_directories(include)

add_executable(6502Emu ${SOURCES})

target_link_libraries(6502Emu SDL2 SDL2_ttf)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
