cmake_minimum_required(VERSION 3.15)

project(lab3)

find_package(OpenGL REQUIRED)

# Add an executable target named 'lab3' that is built from the source
# file 'main.cpp'. This means that CMake will generate build rules to compile
# 'main.cpp' and link it into an executable program named 'lab3'.
add_executable(${PROJECT_NAME} main.cpp Lab3Application.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
) 

target_link_libraries(${PROJECT_NAME} PRIVATE
	glfw 
	glad
	glm
	GLFWApplication
	GeometricTools
	Rendering
	RenderCommands
)

