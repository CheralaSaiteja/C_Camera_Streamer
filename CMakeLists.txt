cmake_minimum_required(VERSION 3.0.0)
project(CameraStreamer VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(libs/cxxopts)

find_package(OpenCV REQUIRED)

file(GLOB SRC src/*.cpp)

add_executable(CameraStreamer ${SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS} include/ libs/cxxopts/include/)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} cxxopts)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG)
endif()