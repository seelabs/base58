cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE "/home/swd/projs/packages/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(xrpl_base58)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#required to build the library
find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)
# Required for testing/benchmarks
find_package(Catch2 REQUIRED)
find_package(benchmark REQUIRED)

set(SOURCE_FILES src/digest.cpp src/tokens.cpp)
add_library(xrpl_base58 SHARED ${SOURCE_FILES})
target_link_libraries(xrpl_base58 PUBLIC Boost::boost OpenSSL::Crypto)
target_compile_options(xrpl_base58 PUBLIC "-ggdb3")
target_link_options(xrpl_base58 PUBLIC "-ggdb3")
target_include_directories(xrpl_base58 PUBLIC src)

add_executable(test src/tests.cpp)
target_link_libraries(test PUBLIC xrpl_base58)
target_compile_options(test PUBLIC "-ggdb3")
target_link_options(test PUBLIC "-ggdb3")
target_include_directories(test PUBLIC src)

add_executable(benchmark src/benchmarks.cpp)
target_link_libraries(benchmark PUBLIC xrpl_base58 benchmark::benchmark)
target_compile_options(benchmark PUBLIC "-ggdb3")
target_link_options(benchmark PUBLIC "-ggdb3")
target_include_directories(benchmark PUBLIC src)
