cmake_minimum_required(VERSION 3.15)

project(assignment)

find_package(OpenGL REQUIRED)

set(SOURCES
  main.cpp
  AssignmentApp.cpp
  Cube.h
  )

add_executable(${PROJECT_NAME} ${SOURCES})

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

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures/cube_texture.png
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/textures/cube_texture.png)

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures/floor_texture.png
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/textures/floor_texture.png)


target_compile_definitions(${PROJECT_NAME} PRIVATE
	TEXTURES_DIR="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/textures/")


