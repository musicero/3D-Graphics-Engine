set(NAME GeometricTools)

add_library(${NAME} INTERFACE)
add_library(Framework::GeometricTools ALIAS ${NAME})

target_sources(${NAME} INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/GeometricTools.cpp
)

target_include_directories(${NAME} INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}
) 

