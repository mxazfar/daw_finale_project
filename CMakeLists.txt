cmake_minimum_required(VERSION 3.10)
project(DAW)

set(CMAKE_CXX_STANDARD 11)

add_executable(DAW 
    main.cpp
    audio_processing.cpp
    audio_processing.hpp
)

if(WIN32)
    target_link_libraries(DAW winmm)
    target_link_libraries(DAW comctl32)
endif()