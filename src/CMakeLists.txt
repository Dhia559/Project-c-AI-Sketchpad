cmake_minimum_required(VERSION 3.16)
project(AI_Sketchpad LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Point to your SFML folder
set(SFML_DIR "C:/SFML-3.0.2/lib/cmake/SFML")

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_executable(AI_Sketchpad main.cpp)
target_link_libraries(AI_Sketchpad PRIVATE SFML::Graphics SFML::Window SFML::System)
    