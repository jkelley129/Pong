cmake_minimum_required(VERSION 4.0)
project(MultiMediaPractice)

include(fetchContent)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_STANDARD 26)
FETCHCONTENT_DECLARE(SFML GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 3.0.1)
FETCHCONTENT_MAKEAVAILABLE(SFML)

add_executable(MultiMediaPractice main.cpp)

target_link_libraries(MultiMediaPractice sfml-graphics)
