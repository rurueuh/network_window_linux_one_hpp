cmake_minimum_required(VERSION 3.9)

project(game VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

file(GLOB_RECURSE helloworld_SRC
     "src/*.hpp"
     "src/*.cpp"
)

foreach (file ${helloworld_SRC})
    set(MYFILE_DIR)
    if (file MATCHES ".*.hpp")
        get_filename_component(MYFILE_DIR ${file} DIRECTORY)
        include_directories(${MYFILE_DIR})
    endif()
endforeach()

add_executable(game ${helloworld_SRC})


# target_link_libraries(game sfml-graphics sfml-window sfml-system sfml-network sfml-audio opengl32 winmm gdi32)
target_link_libraries(game -lws2_32)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
