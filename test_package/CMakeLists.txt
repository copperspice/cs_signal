cmake_minimum_required(VERSION 3.1)
project(PackageTest CXX)

find_package( Threads REQUIRED )
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(example main.cpp peach.cpp )
target_link_libraries(example ${CONAN_LIBS} Threads::Threads)

# CTest is a testing tool that can be used to test your project.
# enable_testing()
# add_test(NAME example
#          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
#          COMMAND example)
