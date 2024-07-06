cmake_minimum_required(VERSION 3.10)
project(DAW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(DAW WIN32
    main.cpp
    audio_processing.cpp
    audio_processing.hpp
    error_handling.cpp 
    error_handling.hpp
)

if(WIN32)
    target_link_libraries(DAW PRIVATE 
        winmm 
        comctl32 
        dbghelp
        user32
        gdi32
        ole32
        oleaut32
        uuid
    )
endif()