cmake_minimum_required(VERSION 3.20)

project(ComputerGraphics_Duck VERSION 0.1 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(externals)

set(DUCK_LINK_LIBS 	PUBLIC glm
              		PUBLIC glfw
              		PUBLIC glad
					PUBLIC imgui
					PUBLIC stb)

set(DUCK_EXECUTABLE_NAME duck)
file(GLOB DUCK_SOURCE_FILES source/*.cpp)

add_executable(${DUCK_EXECUTABLE_NAME} ${DUCK_SOURCE_FILES})
target_link_libraries(${DUCK_EXECUTABLE_NAME} ${DUCK_LINK_LIBS})

