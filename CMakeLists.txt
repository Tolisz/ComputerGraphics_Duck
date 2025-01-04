cmake_minimum_required(VERSION 3.20)

project(ComputerGraphics_Duck VERSION 0.1 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)

option(DUCK_EMBEDDED_SHADERS "All shaders will be embedded into program binary" ON)
option(DUCK_SHOW_DEBUG_CONSOLE "If On should open only window without a console" OFF)

add_subdirectory(externals)
add_subdirectory(utils)

set(DUCK_LINK_LIBS 	PUBLIC glm
              		PUBLIC glfw
              		PUBLIC glad
					PUBLIC imgui
					PUBLIC stb)

set(DUCK_EXECUTABLE_NAME duck)
file(GLOB DUCK_SOURCE_FILES source/*.cpp)

add_executable(${DUCK_EXECUTABLE_NAME} ${DUCK_SOURCE_FILES})
target_link_libraries(${DUCK_EXECUTABLE_NAME} ${DUCK_LINK_LIBS})

if (DUCK_EMBEDDED_SHADERS)
	message("-- DUCK: DUCK_EMBEDDED_SHADERS option is ON")

	target_compile_definitions(${DUCK_EXECUTABLE_NAME} PRIVATE "EMBEDDED_SHADERS")
    shaders_to_embedded_hpp(${DUCK_EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/shaders)
else()
	message("-- DUCK: DUCK_EMBEDDED_SHADERS option is OFF")
endif()

if (MSVC)
	target_compile_options(${DUCK_EXECUTABLE_NAME} PRIVATE /W4)

	if (NOT DUCK_SHOW_DEBUG_CONSOLE)

		message("-- DUCK: DUCK_SHOW_DEBUG_CONSOLE option is OFF")
		set_target_properties(${DUCK_EXECUTABLE_NAME} PROPERTIES 
			LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
	else() 
		message("-- DUCK: DUCK_SHOW_DEBUG_CONSOLE option is ON")
	endif()

else()
	target_compile_options(${DUCK_EXECUTABLE_NAME} PRIVATE -Wall -Wextra -Wpedantic)

	# Is is not that simple ....
	if (NOT DUCK_SHOW_DEBUG_CONSOLE)
		message("-- DUCK: DUCK_SHOW_DEBUG_CONSOLE option is OFF")
	else() 
		message("-- DUCK: DUCK_SHOW_DEBUG_CONSOLE option is ON")
	endif()
endif()