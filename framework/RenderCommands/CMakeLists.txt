set(NAME RenderCommands)

add_library(${NAME} INTERFACE)
add_library(Framework::RenderCommands ALIAS ${NAME})

target_include_directories(${NAME} INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}
) 

