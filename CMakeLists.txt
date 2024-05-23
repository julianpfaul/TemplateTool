cmake_minimum_required(VERSION 3.28)
project(JP_PROJECT)

set(CMAKE_CXX_STANDARD 20)

add_executable(tt main.cpp) # tt = template_tool

install(TARGETS tt
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
