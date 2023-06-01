cmake_minimum_required(VERSION 3.10)
project(OIDC)

set(CMAKE_CXX_STANDARD 14)

# Find OpenSSL on your system
find_package(OpenSSL REQUIRED)

# Find the C++ Rest SDK package
find_package(cpprestsdk REQUIRED)

add_executable(HttpServer http_server.cpp)

# Link the C++ Rest SDK to your target
target_link_libraries(HttpServer PRIVATE cpprestsdk::cpprest OpenSSL::SSL) 
