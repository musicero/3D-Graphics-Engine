cmake_minimum_required(VERSION 3.15)

project(lab2)

find_package(OpenGL REQUIRED)

# Add an executable target named 'lab2' that is built from the source
# file 'main.cpp'. This means that CMake will generate build rules to compile
# 'main.cpp' and link it into an executable program named 'lab2'.
add_executable(${PROJECT_NAME} main.cpp Lab2Application.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
) 

# Specify libraries to link with the 'lab2' target.
# This tells CMake that when it's linking 'lab2', it should also
# link the following libraries:
# - glfw: This is a windowing library commonly used with OpenGL.
# - glad: A library to load OpenGL extensions.
# - OpenGL::GL: This is an imported target for the main OpenGL library
#               provided by the find_package(OpenGL) command.
target_link_libraries(${PROJECT_NAME} PRIVATE
	glfw 
	glad
	glm
	GLFWApplication
	GeometricTools
	Rendering
)

