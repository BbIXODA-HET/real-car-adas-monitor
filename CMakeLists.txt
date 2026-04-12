cmake_minimum_required(VERSION 3.20)
project(RealCarMonitor VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(OpenCV_DIR "C:/opencv/build")
find_package(OpenCV REQUIRED)

set(ONNXRUNTIME_DIR "C:/onnxruntime")
include_directories(${ONNXRUNTIME_DIR}/include)
link_directories(${ONNXRUNTIME_DIR}/lib)

file(GLOB SOURCES "src/*.cpp")

add_executable(RealCarMonitor ${SOURCES})
target_link_libraries(RealCarMonitor ${OpenCV_LIBS} onnxruntime)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(RunTests tests/test_obd_parser.cpp src/obd_parser.cpp src/dms_monitor.cpp)
target_link_libraries(RunTests GTest::gtest_main ${OpenCV_LIBS})