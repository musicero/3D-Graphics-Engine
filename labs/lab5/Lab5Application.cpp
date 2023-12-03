#include "Lab5Application.h"
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

	layout(location = 0) in vec3 i_position;
	layout(location = 1) in vec2 i_tcoords;
  layout(location = 2) in vec3 i_normal;

	out vec2 position2;
	out vec2 vs_tcoords;
	out vec3 vs_position;
  out vec4 vs_normal;
  out vec4 vs_fragPosition;

	uniform mat4 u_Model;
	uniform mat4 u_ViewProjection;

	void main(){
		gl_Position = u_ViewProjection * u_Model * vec4(i_position, 1.0);
		position2 = vec2(i_position.x+0.5, i_position.y+0.5);

		vs_tcoords = i_tcoords;
		vs_position = i_position;
    vs_normal = normalize(u_Model * vec4(i_normal, 1.0));
    vs_fragPosition = u_Model * vec4(i_position, 1.0);
	}
)";

const std::string chessFragmentShader = R"(
	#version 430 core

	layout(binding=0) uniform sampler2D u_floorTextureSampler;

	in vec2 position2;
	in vec2 vs_tcoords;
  in vec4 vs_normal;
  in vec4 vs_fragPosition;
		
  out vec4 color;

	uniform ivec2 selector;	
  uniform float u_ambientStrength = 1.0;
  uniform float u_diffuseStrength;
  uniform vec3 u_lightSourcePosition;
  uniform vec3 u_diffuseColor;

  uniform vec3 u_cameraPosition; 
  uniform float u_specularStrength = 0.5;
  uniform vec3 u_specularColor;

	void main(){
		vec4 m_color;
		ivec2 tileIndex = ivec2(floor(position2 * 8));

		if((tileIndex.x + tileIndex.y) % 2 == 0){
			 m_color = vec4(0.0, 0.0, 0.0, 1.0);
		} else {
			 m_color = vec4(1.0, 1.0, 1.0, 1.0);
		}

		color = mix(m_color, texture(u_floorTextureSampler, vs_tcoords), 0.7);
    color = vec4(u_ambientStrength*color.rgb, color.w);

    vec3 lightDirection = normalize(vec3(u_lightSourcePosition - vs_fragPosition.xyz));
    float diffuseStrength = max(dot(lightDirection, vs_normal.xyz), 0.0f) * u_diffuseStrength;

    vec3 reflectedLight = normalize(reflect(-lightDirection, vs_normal.xyz));
    vec3 observerDirection = normalize(u_cameraPosition - vs_fragPosition.xyz);
    float specFactor = pow(max(dot(observerDirection, reflectedLight), 0.0), 12);
    
    vec3 diffuse = vec3(diffuseStrength)* u_diffuseColor;
    vec3 ambient = vec3(u_ambientStrength);
    vec3 specular = vec3(specFactor * u_specularStrength) * u_specularColor;


    color = vec4(color.rgb * (ambient + diffuse + specular).rgb, 1.0);
	}
)";

const std::string cubeVertexShader = R"(
	#version 430 core

	layout(location = 0) in vec3 i_position;
	layout(location = 1) in vec2 i_tcoords;
  layout(location = 2) in vec3 i_normal;

	out vec2 vs_tcoords;
	out vec3 vs_position;
  out vec4 vs_normal;
  out vec4 vs_normal_model;
  out vec4 vs_fragPosition;

	uniform mat4 u_Model;
	uniform mat4 u_ViewProjection;
  uniform mat4 u_Rotation;

	void main(){
		gl_Position = u_ViewProjection * u_Model * vec4(i_position, 1.0);

		vs_tcoords = i_tcoords;
		vs_position = i_position;
    vs_normal_model = normalize(u_Model    * vec4(i_normal, 1.0));
    vs_normal = normalize(u_Rotation * vec4(i_normal, 1.0));
    vs_fragPosition = u_Model * vec4(i_position, 1.0);
	}
)";

const std::string cubeFragmentShader = R"(
	#version 430 core

	layout(binding = 1) uniform samplerCube uTexture;

	in vec3 vs_position;
  in vec4 vs_normal;
  in vec4 vs_normal_model;
  in vec4 vs_fragPosition;
  
	out vec4 color;
		
	uniform int color_choice;

  uniform float u_ambientStrength = 1.0;
  uniform vec3 u_ambientColor;

  uniform vec3 u_lightSourcePosition;
  uniform float u_diffuseStrength;
  uniform vec3 u_diffuseColor;

  uniform vec3 u_cameraPosition; 
  uniform float u_specularStrength = 0.5;
  uniform vec3 u_specularColor;

	void main(){
		vec4 m_color;
		if(color_choice == 1){
			m_color = vec4(1.0, 0.0, 0.0, 1.0);
		} else if(color_choice==2){
			m_color = vec4(0.0, 1.0, 0.0, 1.0);
		} else if(color_choice==3){
			m_color = vec4(0.0, 0.0, 1.0, 1.0);
		}
		color = mix(m_color, texture(uTexture, vs_position), 0.7);

    vec3 lightDirection = normalize(vec3(u_lightSourcePosition - vs_fragPosition.xyz));
    float diffuseStrength = max(dot(lightDirection, vs_normal.xyz), 0.0f) * u_diffuseStrength;

    vec3 reflectedLight = normalize(reflect(-lightDirection, vs_normal.xyz));
    vec3 observerDirection = normalize(u_cameraPosition - vs_fragPosition.xyz);
    float specFactor = pow(max(dot(observerDirection, reflectedLight), 0.0), 12);
    
    vec3 diffuse = color.rgb * vec3(diffuseStrength)*u_diffuseColor;
    vec3 ambient = color.rgb * vec3(u_ambientStrength)*u_ambientColor;
    vec3 specular = vec3(specFactor * u_specularStrength)* u_specularColor;
    

    //color = vec4(diffuse + ambient + specular, 1.0);
    color = vec4(diffuse, 1.0);
    //color = vec4(specular, 1.0); //only render with specular lighting

	}
)";

glm::ivec2 selector = {0, 0};
glm::fvec3 playerPos = {0.f, -3.f, 3.f};

GLuint CompileShader();
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods);

Lab5Application::Lab5Application(const std::string &name,
                                 const std::string &version)
    : GLFWApplication(name, version) {}

unsigned Lab5Application::Run() {

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  // glEnable(GL_CULL_FACE);
  //  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);

  auto cameraPosition = glm::vec3(0.f, -3.f, 3.f);
  auto lightPosition = glm::vec3(0.f, 2.f, 4.f);

  auto ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
  auto diffuseColor = glm::vec3(1.0f, 1.0f, 1.f);
  auto specularColor = glm::vec3(1.f, 1.f, 1.f);

  float diffuseStrength = 2.0f;
  float specularStrength = 5.0f;
  float ambientStrength = 0.2f;

  PerspectiveCamera camera =
      PerspectiveCamera({45.f, 1000.f, 1000.f, 0.1f, 1000.f}, cameraPosition,
                        {0.f, 0.f, 0.f}, {0.f, 0.f, 1.f});

  auto viewProjection = camera.GetViewProjectionMatrix();

  camera.PrintAttributes();

  GeometricTools::unitShape chessboard = GeometricTools::UnitGrid(4);

  auto chessVertexBuffer = std::make_shared<VertexBuffer>(
      chessboard.vertices.data(), chessboard.vertices.size() * sizeof(GLfloat));
  auto chessBufferLayout = BufferLayout({{ShaderDataType::Float3, "position"},
                                         {ShaderDataType::Float2, "tcoords"},
                                         {ShaderDataType::Float3, "normal"}});

  chessVertexBuffer->SetLayout(chessBufferLayout);

  auto chessVertexArray = std::make_shared<VertexArray>();
  chessVertexArray->AddVertexBuffer(chessVertexBuffer);

  auto chessIndexBuffer = std::make_shared<IndexBuffer>(
      chessboard.indices.data(), chessboard.indices.size());
  chessVertexArray->SetIndexBuffer(chessIndexBuffer);

  std::shared_ptr<Shader> chessboardShader =
      std::make_shared<Shader>(VertexShader, chessFragmentShader, "chessboard");

  // -- chessboard transformation-matrix -- //

  auto scale = glm::scale(glm::mat4(1.0f), {3.0f, 3.0f, 3.0f});
  auto rotate =
      glm::rotate(glm::mat4(1.0f), glm::radians(0.f), {1.f, 0.f, 0.f});
  // auto rotate =
  // glm::rotate(glm::mat4(1.0f), glm::radians(0.f), {0.0f, 0.0f, 0.0f});
  auto translate = glm::translate(glm::mat4(1.0f), {0.0f, 0.0f, -1.0f});

  auto chessboardModelMatrix = translate * rotate * scale;

  chessboardShader->Bind();
  chessboardShader->UploadUniformFloatM4("u_ViewProjection", viewProjection);
  chessboardShader->UploadUniformFloatM4("u_Model", chessboardModelMatrix);

  chessboardShader->UploadUniformFloat("u_ambientStrength", ambientStrength);
  chessboardShader->UploadUniformFloat3("u_ambientColor", ambientColor);

  chessboardShader->UploadUniformFloat3("u_lightSourcePosition", lightPosition);
  chessboardShader->UploadUniformFloat("u_diffuseStrength", diffuseStrength);
  chessboardShader->UploadUniformFloat3("u_diffuseColor", diffuseColor);

  chessboardShader->UploadUniformFloat3("u_cameraPosition", cameraPosition);
  chessboardShader->UploadUniformFloat("u_specularStrength", specularStrength);
  chessboardShader->UploadUniformFloat3("u_specularColor", specularColor);

  // ------- CUBE ------- //

  GeometricTools::unitShape cube = GeometricTools::UnitCubeWNormals();

  auto cubeVertexBuffer = std::make_shared<VertexBuffer>(
      cube.vertices.data(), cube.vertices.size() * sizeof(GLfloat));
  auto cubeBufferLayout = BufferLayout({{ShaderDataType::Float3, "position"},
                                        {ShaderDataType::Float2, "tcoords"},
                                        {ShaderDataType::Float3, "normal"}});

  cubeVertexBuffer->SetLayout(cubeBufferLayout);

  auto cubeVertexArray = std::make_shared<VertexArray>();
  cubeVertexArray->AddVertexBuffer(cubeVertexBuffer);

  auto cubeIndexBuffer =
      std::make_shared<IndexBuffer>(cube.indices.data(), cube.indices.size());
  cubeVertexArray->SetIndexBuffer(cubeIndexBuffer);

  auto cubeShader =
      std::make_shared<Shader>(cubeVertexShader, cubeFragmentShader, "cube");

  auto cubeScale = glm::scale(glm::mat4(1.0f), {1.0f, 1.0f, 1.f});
  auto cubeRotate =glm::rotate(glm::mat4(1.f), glm::radians(0.0f), {0.0f, 1.0f, 1.0f});
  auto cubeTranslate = glm::translate(glm::mat4(1.0f), {0.5f, 0.0f, -1.0f});

  auto cubeModelMatrix = cubeTranslate * cubeRotate * cubeScale;

  cubeShader->Bind();
  cubeShader->UploadUniformFloatM4("u_ViewProjection", viewProjection);
  cubeShader->UploadUniformFloatM4("u_Model", cubeModelMatrix);
  cubeShader->UploadUniformFloatM4("u_Rotation", cubeRotate);

  cubeShader->UploadUniformFloat("u_ambientStrength", ambientStrength);
  cubeShader->UploadUniformFloat3("u_ambientColor", ambientColor);

  cubeShader->UploadUniformFloat3("u_lightSourcePosition", lightPosition);
  cubeShader->UploadUniformFloat("u_diffuseStrength", diffuseStrength);
  cubeShader->UploadUniformFloat3("u_diffuseColor", diffuseColor);

  cubeShader->UploadUniformFloat3("u_cameraPosition", cameraPosition);
  cubeShader->UploadUniformFloat("u_specularStrength", specularStrength);
  cubeShader->UploadUniformFloat3("u_specularColor", specularColor);

  // ---- Textures ---- //
  TextureManager *textureManager = TextureManager::GetInstance();

  textureManager->LoadTexture2DRGBA(
      "chessTex", std::string(TEXTURES_DIR) + std::string("floor_texture.png"),
      0);
  textureManager->LoadCubeMapRGBA(
      "cubeTex", std::string(TEXTURES_DIR) + std::string("cube_texture.png"),
      1);

  // glDepthFunc(GL_LESS);

  float time = 0.0f;
  RenderCommands::SetClearColor({0.4, 0.5, 0.7});
  while (!glfwWindowShouldClose(window)) {

    RenderCommands::Clear();

    chessVertexArray->Bind();
    chessboardShader->Bind();
    chessboardShader->UploadUniformInt2("selector", selector);
    //RenderCommands::SetSolidMode();
    RenderCommands::DrawIndex(chessVertexArray);

    cubeVertexArray->Bind();
    cubeShader->Bind();
    cubeShader->UploadUniformInt("color_choice", 1);
    //RenderCommands::SetSolidMode();
    RenderCommands::DrawIndex(cubeVertexArray);

    cubeRotate =
        glm::rotate(glm::mat4(1.0f), glm::radians(time), {1.f, 0.0f, 0.0f});

    cubeModelMatrix = cubeTranslate * cubeRotate * cubeScale;
    cubeShader->UploadUniformFloatM4("u_Model", cubeModelMatrix);
    cubeShader->UploadUniformFloatM4("u_Rotation", cubeRotate);

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
