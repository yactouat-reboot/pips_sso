cmake_minimum_required(VERSION 3.10)
project(PIPS_SSO)

set(CMAKE_CXX_STANDARD 14)

# Find OpenSSL on your system
find_package(OpenSSL REQUIRED)

# Find the C++ Rest SDK package
find_package(cpprestsdk REQUIRED)

# tell cmake where to find the include dir
include_directories(./include)

# Create a list of all source files in the src directory
file(GLOB SOURCES "./src/*.cpp" "./src/*/*.cpp")

add_executable(HttpDevService ${SOURCES})

# Link the C++ Rest SDK to your target
target_link_libraries(HttpDevService PRIVATE cpprestsdk::cpprest OpenSSL::SSL) 
