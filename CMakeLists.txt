cmake_minimum_required(VERSION 3.6)
project(CameraObjectLocation)

set(CMAKE_CXX_STANDARD 11)

#set(SOURCE_FILES src/main.cpp src/pixels_to_centimeters/Converter.hpp src/pixels_to_centimeters/Converter.cpp src/ball_find/BallFinder.hpp src/ball_find/BallFinder.cpp)
#add_executable(CameraObjectLocation ${SOURCE_FILES})

file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)
include_directories(${HEADERS})
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(CameraObjectLocation ${OpenCV_LIBS})