cmake_minimum_required(VERSION 3.16)
project(limit_order_book LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Core library: tách logic OrderBook khỏi CLI và Tests
add_library(lob_core
    src/OrderBook.cpp
)
target_include_directories(lob_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(lob_core PRIVATE /W4)
else()
    target_compile_options(lob_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# CLI demo executable
add_executable(lob_cli
    src/main.cpp
)
target_link_libraries(lob_cli PRIVATE lob_core)

# Automated unit tests
enable_testing()
add_executable(lob_tests
    tests/test_orderbook.cpp
)
target_link_libraries(lob_tests PRIVATE lob_core)

if(MSVC)
    target_compile_options(lob_tests PRIVATE /W4)
else()
    target_compile_options(lob_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME OrderBookTests COMMAND lob_tests)
