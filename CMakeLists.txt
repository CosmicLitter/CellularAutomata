cmake_minimum_required(VERSION 3.30)

project(
  Automata
  VERSION 0.1
  DESCRIPTION "Roguelike Engine"
  LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++ Standard flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build as Debug
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
  message(STATUS "Build type not specified. Defaulting to Debug")
endif()

# Compile options
add_compile_options(-Wall -Wextra -Wpedantic)

# IDE folder support
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Include cmake modules path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Find SDL packages
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Gather source files
file(GLOB_RECURSE SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

add_executable(Automata ${SOURCE_FILES})

# Add ImGui
file(GLOB IMGUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/*.cpp)
add_library(imgui STATIC ${IMGUI_SOURCES})

# Include SDL2 directories in ImGui
target_include_directories(imgui PRIVATE
  ${SDL2_INCLUDE_DIRS}
)

target_include_directories(Automata PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${SDL2_INCLUDE_DIRS}
  ${SDL2_IMAGE_INCLUDE_DIRS}
  ${SDL2_TTF_INCLUDE_DIRS}
)

target_link_libraries(Automata PRIVATE
  ${SDL2_LIBRARIES}
  ${SDL2_IMAGE_LIBRARIES}
  ${SDL2_TTF_LIBRARIES}
  imgui
)
