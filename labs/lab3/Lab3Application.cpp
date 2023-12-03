#include "Lab3Application.h"
#include "GeometricTools.h"
#include "IndexBuffer.h"
#include "RenderCommands.h"
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

#include <iostream>
#include <memory>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtc/matrix_transform.hpp"

const std::string VertexShader = R"(
	#version 430 core
	layout(location = 0) in vec3 position;
	layout(location = 1) in vec3 s_color;

	uniform mat4 u_Model;
	uniform mat4 u_View;
	uniform mat4 u_Projection;

	out vec2 position2;
	void main(){
		gl_Position = u_Projection * u_View * u_Model * vec4(position, 1.0);
		position2 = vec2(position.x+0.5, position.y+0.5);
	}
)";

const std::string chessFragmentShader = R"(
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
	}
)";

const std::string cubeFragmentShader = R"(
	#version 430 core

	in vec2 position2;
	out vec4 color;
		
	//uniform vec4 u_Color;

	void main(){
		color = vec4(0.0, 1.0, 1.0, 0.5);
	}
)";

const std::string gcubeFragmentShader = R"(
	#version 430 core

	in vec2 position2;
	out vec4 color;
		
	//uniform vec4 u_Color;

	void main(){
		color = vec4(1.0, 0.0, 0.0, 0.5);
	}
)";

glm::ivec2 selector = {0, 0};

GLuint CompileShader();
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods);

Lab3Application::Lab3Application(const std::string &name,
                                 const std::string &version)
    : GLFWApplication(name, version) {}

unsigned Lab3Application::Run() {

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  glm::vec3 cameraPosition(0.0f, 0.0f, 5.0f);
  glm::vec3 cameraOrientation(0.0f, 0.0f, 0.0f);
  glm::vec3 cameraUpVector(0.0f, 1.0f, 0.0f);

  auto projection = glm::perspective(
      glm::radians(45.0f), // Field of view, angle from bottom-top
      1.f,                 // aspect ratio
      1.f,                 // near clipping plane
      -10.f                // far clipping plane
  );

  auto view = glm::lookAt(cameraPosition, cameraOrientation, cameraUpVector);

  GeometricTools::unitShape chessboard = GeometricTools::UnitGrid(4);

  auto chessVertexBuffer = std::make_shared<VertexBuffer>(
      chessboard.vertices.data(), chessboard.vertices.size() * sizeof(GLfloat));
  auto chessBufferLayout = BufferLayout({{ShaderDataType::Float3, "position"},
                                         {ShaderDataType::Float3, "color"}});

  chessVertexBuffer->SetLayout(chessBufferLayout);

  auto chessVertexArray = std::make_shared<VertexArray>();
  chessVertexArray->AddVertexBuffer(chessVertexBuffer);

  auto chessIndexBuffer = std::make_shared<IndexBuffer>(
      chessboard.indices.data(), chessboard.indices.size());
  chessVertexArray->SetIndexBuffer(chessIndexBuffer);

  std::shared_ptr<Shader> chessboardShader =
      std::make_shared<Shader>(VertexShader, chessFragmentShader);

  // -- chessboard transformation-matrix -- //

  auto scale = glm::scale(glm::mat4(1.0f), {3.0f, 3.0f, 3.0f});
  auto rotate =
      glm::rotate(glm::mat4(1.0f), glm::radians(-60.f), {1.0f, 0.0f, 0.0f});
  auto translate = glm::translate(glm::mat4(1.0f), {0.0f, 0.0f, -1.0f});

  auto chessboardModelMatrix = translate * rotate * scale;

  chessboardShader->Bind();
  chessboardShader->UploadUniformFloatM4("u_Projection", projection);
  chessboardShader->UploadUniformFloatM4("u_View", view);
  chessboardShader->UploadUniformFloatM4("u_Model", chessboardModelMatrix);

  // ------- CUBE ------- //

  GeometricTools::unitShape cube = GeometricTools::UnitCube();

  auto cubeVertexBuffer = std::make_shared<VertexBuffer>(
      cube.vertices.data(), cube.vertices.size() * sizeof(GLfloat));
  auto cubeBufferLayout = BufferLayout({{ShaderDataType::Float3, "position"},
                                        {ShaderDataType::Float3, "color"}});

  cubeVertexBuffer->SetLayout(cubeBufferLayout);

  auto cubeVertexArray = std::make_shared<VertexArray>();
  cubeVertexArray->AddVertexBuffer(cubeVertexBuffer);

  auto cubeIndexBuffer =
      std::make_shared<IndexBuffer>(cube.indices.data(), cube.indices.size());
  cubeVertexArray->SetIndexBuffer(cubeIndexBuffer);

  std::shared_ptr<Shader> cubeShader =
      std::make_shared<Shader>(VertexShader, cubeFragmentShader);
  std::shared_ptr<Shader> gcubeShader =
      std::make_shared<Shader>(VertexShader, gcubeFragmentShader);
  auto cubeScale = glm::scale(glm::mat4(1.0f), {1.0f, 1.0f, 1.0f});
  auto cubeRotate =
      glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), {1.0f, 1.0f, 0.0f});
  auto cubeTranslate = glm::translate(glm::mat4(1.0f), {0.0f, 0.0f, 0.0f});

  auto cubeModelMatrix = cubeTranslate * cubeRotate * cubeScale;

  cubeShader->Bind();
  cubeShader->UploadUniformFloatM4("u_Projection", projection);
  cubeShader->UploadUniformFloatM4("u_View", view);
  cubeShader->UploadUniformFloatM4("u_Model", cubeModelMatrix);

  gcubeShader->Bind();
  gcubeShader->UploadUniformFloatM4("u_Projection", projection);
  gcubeShader->UploadUniformFloatM4("u_View", view);
  gcubeShader->UploadUniformFloatM4("u_Model", cubeModelMatrix);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glLineWidth(7);
  float time = 0.0f;
  RenderCommands::SetClearColor({0.4, 0.5, 0.7});
  while (!glfwWindowShouldClose(window)) {

    RenderCommands::Clear();

    chessVertexArray->Bind();
    chessboardShader->Bind();
    chessboardShader->UploadUniformInt2("selector", selector);
    RenderCommands::SetSolidMode();
    RenderCommands::DrawIndex(chessVertexArray);

    cubeVertexArray->Bind();
    cubeShader->Bind();
    RenderCommands::SetSolidMode();
    RenderCommands::DrawIndex(cubeVertexArray);

    gcubeShader->Bind();
    RenderCommands::SetWireframeMode();
    RenderCommands::DrawIndex(cubeVertexArray);

    cubeRotate =
        glm::rotate(glm::mat4(1.0f), glm::radians(time), {1.0f, 1.0f, 0.0f});

    cubeModelMatrix = cubeTranslate * cubeRotate * cubeScale;
    cubeShader->UploadUniformFloatM4("u_Model", cubeModelMatrix);
    gcubeShader->UploadUniformFloatM4("u_Model", cubeModelMatrix);

    time += 1.0f;

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {

  if (((key == GLFW_KEY_W) || (key == GLFW_KEY_UP)) && action == GLFW_PRESS &&
      selector.y < 7)
    selector.y++;
  if (((key == GLFW_KEY_A) || (key == GLFW_KEY_LEFT)) && action == GLFW_PRESS &&
      selector.x > 0)
    selector.x--;
  if (((key == GLFW_KEY_S) || (key == GLFW_KEY_DOWN)) && action == GLFW_PRESS &&
      selector.y > 0)
    selector.y--;
  if (((key == GLFW_KEY_D) || (key == GLFW_KEY_RIGHT)) &&
      action == GLFW_PRESS && selector.x < 7)
    selector.x++;
}
