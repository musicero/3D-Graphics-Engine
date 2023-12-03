#include "Lab4Application.h"
#include "GeometricTools.h"
#include "IndexBuffer.h"
#include "OrthographicCamera.h"
#include "PerspectiveCamera.h"
#include "RenderCommands.h"
#include "Shader.h"
#include "TextureManager.h"
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
	layout(location = 1) in vec2 tcoords;
	out vec2 vs_tcoords;
	out vec3 vs_position;

	uniform mat4 u_Model;
	uniform mat4 u_ViewProjection;

	out vec2 position2;
	void main(){
		gl_Position = u_ViewProjection * u_Model * vec4(position, 1.0);
		position2 = vec2(position.x+0.5, position.y+0.5);

		vs_tcoords = tcoords;
		vs_position = position;
	}
)";

const std::string chessFragmentShader = R"(
	#version 430 core

	in vec2 position2;
	in vec2 vs_tcoords;
	layout(binding=0) uniform sampler2D u_floorTextureSampler;
		
	out vec4 color;
	uniform ivec2 selector;	

	void main(){
		vec4 m_color;
		ivec2 tileIndex = ivec2(floor(position2 * 8));

		if((tileIndex.x + tileIndex.y) % 2 == 0){
			 m_color = vec4(0.0, 0.0, 0.0, 1.0);
		} else {
			 m_color = vec4(1.0, 1.0, 1.0, 1.0);
		}
		color = mix(m_color, texture(u_floorTextureSampler, vs_tcoords), 0.7);

	}
)";

const std::string cubeFragmentShader = R"(
	#version 430 core

	layout(binding = 1) uniform samplerCube uTexture;

	in vec3 vs_position;
	out vec4 finalColor;
		
	uniform int color_choice;

	void main(){
		vec4 m_color;
		if(color_choice == 1){
			m_color = vec4(1.0, 0.0, 0.0, 1.0);
		} else if(color_choice==2){
			m_color = vec4(0.0, 1.0, 0.0, 1.0);
		} else if(color_choice==3){
			m_color = vec4(0.0, 0.0, 1.0, 1.0);
		}
		vec4 color = mix(m_color, texture(uTexture, vs_position), 0.7);
		finalColor = vec4(color.rgb, 0.7);

	}
)";

glm::ivec2 selector = {0, 0};

GLuint CompileShader();
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods);

Lab4Application::Lab4Application(const std::string &name,
                                 const std::string &version)
    : GLFWApplication(name, version) {}

unsigned Lab4Application::Run() {

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  /*
  glm::vec3 cameraPosition(0.0f, 0.0f, 5.0f);
  glm::vec3 cameraOrientation(0.0f, 0.0f, 0.0f);
  glm::vec3 cameraUpVector(0.0f, 1.0f, 0.0f);

  auto projection = glm::perspective(
          glm::radians(45.0f), // Field of view, angle from bottom-top
          1.f, 		// aspect ratio
          1.f, 		// near clipping plane
          -10.f   	// far clipping plane
  );

  auto view = glm::lookAt(
          cameraPosition,
          cameraOrientation,
          cameraUpVector
  );

  */

  PerspectiveCamera camera =
      PerspectiveCamera({45.f, 1000.f, 1000.f, 0.1f, 1000.f}, {5.f, 5.f, 5.f},
                        {0.f, 0.f, 0.f}, {0.f, 0.f, 1.f});

  auto viewProjection = camera.GetViewProjectionMatrix();

  camera.PrintAttributes();

  GeometricTools::unitShape chessboard = GeometricTools::UnitGrid(4);

  auto chessVertexBuffer = std::make_shared<VertexBuffer>(
      chessboard.vertices.data(), chessboard.vertices.size() * sizeof(GLfloat));
  auto chessBufferLayout = BufferLayout({{ShaderDataType::Float3, "position"},
                                         {ShaderDataType::Float2, "tcoords"}});

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
      glm::rotate(glm::mat4(1.0f), glm::radians(0.f), {0.f, 0.f, 1.f});
  // auto rotate =
  glm::rotate(glm::mat4(1.0f), glm::radians(-60.f), {1.0f, 0.0f, 0.0f});
  auto translate = glm::translate(glm::mat4(1.0f), {0.0f, 0.0f, -1.0f});

  auto chessboardModelMatrix = translate * rotate * scale;

  chessboardShader->Bind();
  chessboardShader->UploadUniformFloatM4("u_ViewProjection", viewProjection);
  chessboardShader->UploadUniformFloatM4("u_Model", chessboardModelMatrix);

  // ------- CUBE ------- //

  GeometricTools::unitShape cube = GeometricTools::UnitCube();

  auto cubeVertexBuffer = std::make_shared<VertexBuffer>(
      cube.vertices.data(), cube.vertices.size() * sizeof(GLfloat));
  auto cubeBufferLayout = BufferLayout({{ShaderDataType::Float3, "position"},
                                        {ShaderDataType::Float2, "tcoords"}});

  cubeVertexBuffer->SetLayout(cubeBufferLayout);

  auto cubeVertexArray = std::make_shared<VertexArray>();
  cubeVertexArray->AddVertexBuffer(cubeVertexBuffer);

  auto cubeIndexBuffer =
      std::make_shared<IndexBuffer>(cube.indices.data(), cube.indices.size());
  cubeVertexArray->SetIndexBuffer(cubeIndexBuffer);

  std::shared_ptr<Shader> cubeShader =
      std::make_shared<Shader>(VertexShader, cubeFragmentShader);

  auto cubeScale = glm::scale(glm::mat4(1.0f), {1.0f, 1.0f, 1.0f});
  auto cubeRotate =
      glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), {1.0f, 1.0f, 0.0f});
  auto cubeTranslate = glm::translate(glm::mat4(1.0f), {0.0f, 0.0f, 0.0f});

  auto cubeModelMatrix = cubeTranslate * cubeRotate * cubeScale;

  cubeShader->Bind();
  cubeShader->UploadUniformFloatM4("u_ViewProjection", viewProjection);
  cubeShader->UploadUniformFloatM4("u_Model", cubeModelMatrix);

  // ---- Textures ---- //
  TextureManager *textureManager = TextureManager::GetInstance();

  textureManager->LoadTexture2DRGBA(
      "chessTex", std::string(TEXTURES_DIR) + std::string("floor_texture.png"),
      0);
  textureManager->LoadCubeMapRGBA(
      "cubeTex", std::string(TEXTURES_DIR) + std::string("cube_texture.png"),
      1);

  // GLuint chessTex =
  // this->LoadTextures(std::string(TEXTURES_DIR)+std::string("floor_texture.png"),
  // 0); GLuint cubeTex =
  // this->LoadCubeMap(std::string(TEXTURES_DIR)+std::string("cube_texture.png"),
  // 1);

  // glDepthFunc(GL_LESS);

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
    cubeShader->UploadUniformInt("color_choice", 1);
    RenderCommands::SetSolidMode();
    RenderCommands::DrawIndex(cubeVertexArray);

    cubeRotate =
        glm::rotate(glm::mat4(1.0f), glm::radians(time), {1.0f, 1.0f, 0.0f});

    cubeModelMatrix = cubeTranslate * cubeRotate * cubeScale;
    cubeShader->UploadUniformFloatM4("u_Model", cubeModelMatrix);

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
