cmake_minimum_required(VERSION 3.28.3)
project(sand VERSION 0.1.0)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)


add_executable(sand
    src/config.h
    src/config.cpp
    src/main.cpp
    src/factory.cpp
    src/glad.c
    src/shader.cpp
    

)

target_compile_options(sand PRIVATE -fsanitize=address)
target_link_options(sand PRIVATE -fsanitize=address)

# Set the include directories
target_include_directories(sand
    PRIVATE
    dependencies          
)

# Link with required libraries
target_link_libraries(sand
    glfw
    OpenGL::GL
)
