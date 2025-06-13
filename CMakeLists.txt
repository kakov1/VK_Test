cmake_minimum_required(VERSION 3.14)
project(Metrics)

add_subdirectory(src)

add_executable(main
  main.cpp
)

target_compile_definitions(main PRIVATE 
    LOG_PATH="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(main Metrics)