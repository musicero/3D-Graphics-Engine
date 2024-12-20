#include "Lab6Application.h"
#include "GeometricTools.h"
#include "IndexBuffer.h"
#include "PerspectiveCamera.h"
#include "RenderCommands.h"
#include "Shader.h"
#include "TextureManager.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

#include "shaders/chessFragment.h"
#include "shaders/cubeFragment.h"
#include "shaders/cubeVertex.h"
#include "shaders/vertex.h"

#include <memory>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtc/matrix_transform.hpp"

glm::ivec2 selector = {0, 0};
glm::fvec3 playerPos = {0.f, -3.f, 3.f};

GLuint CompileShader();
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods);

Lab6Application::Lab6Application(const std::string &name,
                                 const std::string &version)
    : GLFWApplication(name, version) {}

unsigned Lab6Application::Run() {

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

  std::shared_ptr<Shader> chessboardShader = std::make_shared<Shader>(
      chessVertexShader, chessFragmentShader, "chessboard");

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
  auto cubeRotate =
      glm::rotate(glm::mat4(1.f), glm::radians(0.0f), {0.0f, 1.0f, 1.0f});
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
    // RenderCommands::SetSolidMode();
    RenderCommands::DrawIndex(chessVertexArray);

    cubeVertexArray->Bind();
    cubeShader->Bind();
    cubeShader->UploadUniformInt("color_choice", 1);
    // RenderCommands::SetSolidMode();
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
