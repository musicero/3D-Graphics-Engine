#include "Lab2Application.h"
#include "GeometricTools.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>

const std::string VertexShader = R"(
	#version 430 core
	layout(location = 0) in vec2 position;
	layout(location = 1) in vec3 s_color;
	out vec2 position2;
	void main(){
		gl_Position = vec4(position, 0.0, 1.0);
		position2 = vec2(position.x+0.5, position.y+0.5);
	}
)";

const std::string FragmentShader = R"(
	#version 430 core

	in vec2 position2;
	uniform ivec2 selector;	
	out vec4 color;
		
	//uniform vec4 u_Color;

	void main(){
		ivec2 tileIndex = ivec2(floor(position2 * 8));
		if((tileIndex.x + tileIndex.y) % 2 == 0){
			color = vec4(0.0, 0.0, 0.0, 1.0);
		} else {
			color = vec4(1.0, 1.0, 1.0, 1.0);
		}
		if((tileIndex.x == selector.x) && (tileIndex.y == selector.y)) {
			color = vec4(0.0, 1.0, 1.0, 1.0);
		}
	}
)";


glm::ivec2 selector = {0, 0};

GLuint CompileShader();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

Lab2Application::Lab2Application(const std::string& name, const std::string& version) 
	: GLFWApplication(name, version){
}

unsigned Lab2Application::Run(){

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	int size = 0;
	GeometricTools::unitShape chessboard = GeometricTools::UnitGrid(8);

	auto gridIndexBuffer = std::make_shared<IndexBuffer>(chessboard.indices.data(), chessboard.indices.size());
	auto gridBufferLayout = BufferLayout({{ShaderDataType::Float2, "position"}, {ShaderDataType::Float3, "color"}});
	auto gridVertexBuffer = std::make_shared<VertexBuffer>(chessboard.vertices.data(), chessboard.vertices.size() * sizeof(chessboard.vertices[0]));
	gridVertexBuffer->SetLayout(gridBufferLayout);
	auto vertexArray = std::make_shared<VertexArray>();
	vertexArray->AddVertexBuffer(gridVertexBuffer);
	vertexArray->SetIndexBuffer(gridIndexBuffer);

	std::shared_ptr<Shader> chessboardShader = std::make_shared<Shader>(VertexShader, FragmentShader);

	glClearColor(1.0f, 0.5f, 0.5f, 1.0f);
	while(!glfwWindowShouldClose(window)){

		glClear(GL_COLOR_BUFFER_BIT);
		vertexArray->Bind();

		chessboardShader->Bind();

		chessboardShader->UploadUniformInt2("selector", selector);

		glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, (const void*)0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){

	if(((key == GLFW_KEY_W)||(key==GLFW_KEY_UP)) && action == GLFW_PRESS && selector.y<7) selector.y++;
	if(((key == GLFW_KEY_A)||(key==GLFW_KEY_LEFT)) && action == GLFW_PRESS && selector.x>0) selector.x--;
	if(((key == GLFW_KEY_S)||(key==GLFW_KEY_DOWN)) && action == GLFW_PRESS && selector.y>0) selector.y--;
	if(((key == GLFW_KEY_D)||(key==GLFW_KEY_RIGHT)) && action == GLFW_PRESS && selector.x<7) selector.x++;
}
