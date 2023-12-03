set(NAME Rendering)

add_library(${NAME})
add_library(Framework::Rendering ALIAS ${NAME})

target_sources(${NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/VertexBuffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/IndexBuffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/VertexArray.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Shader.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/TextureManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/PerspectiveCamera.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/OrthographicCamera.cpp

)

target_include_directories(${NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
) 

target_link_libraries(${NAME} PUBLIC
	glm
	glad
	stb
)
target_compile_definitions(${NAME} PRIVATE 
	STB_IMAGE_IMPLEMENTATION
)
