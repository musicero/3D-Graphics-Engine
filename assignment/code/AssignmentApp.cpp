#include "AssignmentApp.h"
#include "Cube.h"
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

	out vec2 vs_tcoords;
	out vec3 vs_position;
	out vec2 position2;

	uniform mat4 u_Model;
	uniform mat4 u_ViewProjection;

	void main(){
		gl_Position = u_ViewProjection * u_Model * vec4(i_position, 1.0);
		position2 = vec2(i_position.x+0.5, i_position.y+0.5);

		vs_tcoords = i_tcoords;
		vs_position = i_position;
	}
)";

const std::string chessFragmentShader = R"(
	#version 430 core

	layout(binding=0) uniform sampler2D u_floorTextureSampler;

	in vec2 position2;
	in vec2 vs_tcoords;
  in vec4 vs_fragPosition;
		
  out vec4 color;

	uniform ivec2 u_selector;	
  uniform bool u_usingAdvancedShaders;

	void main(){
		vec4 base_color;
		ivec2 tile_index = ivec2(floor(position2 * 8));

		if((tile_index.x + tile_index.y) % 2 == 0)
		  base_color = vec4(0.0);
		else 
		  base_color = vec4(1.0);
		

		if(tile_index == u_selector) 
			base_color = vec4(0.0, 1.0, 1.0, 1.0);
  
    if(u_usingAdvancedShaders)
		  color = mix(base_color, texture(u_floorTextureSampler, vs_tcoords), 0.7);
    else 
      color = base_color;
    
	}
)";

const std::string cubeFragmentShader = R"(
	#version 430 core

	layout(binding = 1) uniform samplerCube uTexture;

	in vec3 vs_position;
  
	out vec4 color;
		
	uniform vec3 u_baseColor;
  uniform bool u_usingAdvancedShaders;


	void main(){
    vec4 baseColor = vec4(u_baseColor, 1.0);
    color = baseColor;
    if(u_usingAdvancedShaders){
		  color = mix(baseColor, texture(uTexture, vs_position), 0.7);
    }

	}
)";

float deltaTime = 0;
float lastTime = 0;

bool pressedKey[256];

glm::ivec2 selector = {0, 0};
bool selectorPressed = false;
glm::ivec2 selectedTileIndex = {-1, -1};
bool tileIsSelected = false;

bool usingAdvancedShaders = true;

struct Tile {
  std::shared_ptr<Cube> cube;

  Tile() { cube = nullptr; }
};

GLuint CompileShader();
glm::vec3 calculateCameraPosition(float angle, float zoom,
                                  glm::vec3 intialPosition);
void updateDeltaTime();
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods);

AssignmentApp::AssignmentApp(const std::string &name,
                             const std::string &version)
    : GLFWApplication(name, version) {}

unsigned AssignmentApp::Run() {

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glEnable(GL_DEPTH_TEST);

  float globalScaleMultiplier = 3;

  float cameraAngle = 0.f;
  float cameraZoom = 1.f;
  auto initialPosition = glm::vec3(-4, -4, 2);
  auto cameraPosition =
      calculateCameraPosition(cameraAngle, cameraZoom, initialPosition);

  auto cameraLookAt = glm::vec3(0.f);
  auto cameraUpVector = glm::vec3(0, 0, 1);
  auto camera = PerspectiveCamera({45.f, 800.f, 600.f, 0.1f, 1000.f},
                                  cameraPosition, cameraLookAt, cameraUpVector);

  auto viewProjection = camera.GetViewProjectionMatrix();

  // -- Game Board Logic Array -- //
  int boardSize = 8;
  Tile gameboard[boardSize][boardSize];

  // -- Chessboard -- //
  GeometricTools::unitShape chessboard = GeometricTools::UnitGrid(boardSize);

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

  auto chessboardShader =
      std::make_shared<Shader>(VertexShader, chessFragmentShader, "Chessboard");

  // -- chessboard transformation-matrix -- //

  auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(globalScaleMultiplier));

  auto rotate =
      glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(1, 0, 0));

  auto translate = glm::translate(glm::mat4(1), glm::vec3(0));

  auto chessboardModelMatrix = translate * rotate * scale;

  chessboardShader->Bind();
  chessboardShader->UploadUniformFloatM4("u_ViewProjection", viewProjection);
  chessboardShader->UploadUniformFloatM4("u_Model", chessboardModelMatrix);

  // ------- CUBES ------- //

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
      std::make_shared<Shader>(VertexShader, cubeFragmentShader, "Cube");

  for (int y = 0; y < boardSize; y++) {
    for (int x = 0; x < boardSize; x++) {
      Tile *tile = &gameboard[y][x];
      // 3.f is the scaling factor on the chessboard
      if (y <= 1) {
        tile->cube = std::make_shared<Cube>(cubeVertexArray, cubeShader);
        tile->cube->SetColor(glm::vec3(0, 0, 1));
        tile->cube->team = Blue;

        tile->cube->coords = glm::ivec2(x, y);
        tile->cube->globalScale = globalScaleMultiplier;
      }
      if (y >= 6) {
        tile->cube = std::make_shared<Cube>(cubeVertexArray, cubeShader);
        tile->cube->SetColor(glm::vec3(1, 0, 0));
        tile->cube->team = Red;

        tile->cube->coords = glm::ivec2(x, y);
        tile->cube->globalScale = globalScaleMultiplier;
      }
    }
  }

  // ---- Textures ---- //
  TextureManager *textureManager = TextureManager::GetInstance();

  textureManager->LoadTexture2DRGBA(
      "chessTex", std::string(TEXTURES_DIR) + std::string("floor_texture.png"),
      0);

  textureManager->LoadCubeMapRGBA(
      "cubeTex", std::string(TEXTURES_DIR) + std::string("cube_texture.png"),
      1);

  RenderCommands::SetClearColor({1.3, 1.3, 1.3});

  while (!glfwWindowShouldClose(window)) {
    updateDeltaTime();
    RenderCommands::Clear();

    // == Camera control handling == //
    if (pressedKey[GLFW_KEY_H] || pressedKey[GLFW_KEY_L] ||
        pressedKey[GLFW_KEY_O] || pressedKey[GLFW_KEY_P]) {

      if (pressedKey[GLFW_KEY_H]) {
        cameraAngle += 1.f * deltaTime;
      }
      if (pressedKey[GLFW_KEY_L]) {
        cameraAngle -= 1.f * deltaTime;
      }

      if (pressedKey[GLFW_KEY_P] && cameraZoom >= 0.2)
        cameraZoom -= 1.f * deltaTime;

      if (pressedKey[GLFW_KEY_O] && cameraZoom <= 2.0)
        cameraZoom += 1.f * deltaTime;

      glm::vec3 currentPosition =
          calculateCameraPosition(cameraAngle, cameraZoom, initialPosition);

      camera.SetPosition(currentPosition);
    }

    chessVertexArray->Bind();
    chessboardShader->UploadUniformInt2("u_selector", selector);
    chessboardShader->UploadUniformInt("u_usingAdvancedShaders",
                                       usingAdvancedShaders);
    chessboardShader->UploadUniformFloatM4("u_ViewProjection",
                                           camera.GetViewProjectionMatrix());
    RenderCommands::DrawIndex(chessVertexArray);

    // == Pastes cube if tile is empty == //
    if (selectorPressed && tileIsSelected) {
      selectorPressed = false;
      auto *tile = &gameboard[selector.y][selector.x];
      auto *selectedTile = &gameboard[selectedTileIndex.y][selectedTileIndex.x];
      if (tile->cube) {
        tileIsSelected = false;

        // no need to check if cube exists because its done when selecting
        selectedTile->cube->selected = false;
      } else {
        tileIsSelected = false;
        // assign the selected cube to the new tile
        tile->cube = selectedTile->cube;
        selectedTile->cube = nullptr;

        // update the cubes coordinates selected-status
        tile->cube->coords = selector;
        tile->cube->selected = false;
      }
    }

    // == Copies tile index if cube is present == //
    if (selectorPressed) {
      selectorPressed = false;
      auto &cube = gameboard[selector.y][selector.x].cube;
      if (cube) {
        cube->selected = true;
        selectedTileIndex = selector;
        tileIsSelected = true;
      }
    }

    // == Rendering Each Cube == //
    for (int y = 0; y < boardSize; y++) {
      for (int x = 0; x < boardSize; x++) {
        Tile *tile = &gameboard[y][x];

        if (tile->cube) {
          auto cube = tile->cube;
          cube->advancedShaders = usingAdvancedShaders;
          cube->GetVertexArray()->Bind();
          cube->CalculatePosition(boardSize);

          if (cube->team == Blue)
            cube->SetColor(glm::vec3(0, 0, 1));
          else
            cube->SetColor(glm::vec3(1, 0, 0));

          if (selector == glm::ivec2(x, y))
            cube->SetColor(glm::vec3(1, 1, 0));
          if (cube->selected)
            cube->SetColor(glm::vec3(1, 0.8, 0.7));

          cube->SetUniforms(camera.GetViewProjectionMatrix());

          RenderCommands::DrawIndex(cube->GetVertexArray());
        }
      }
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
    if (pressedKey[GLFW_KEY_Q])
      glfwSetWindowShouldClose(window, GL_TRUE);
  }

  // glfwDestroyWindow(window);
  glfwSetKeyCallback(window, NULL);
  glfwTerminate();
  return 0;
}

glm::vec3 calculateCameraPosition(float angle, float zoom,
                                  glm::vec3 initialPosition) {
  return {initialPosition.x * glm::sin(angle) * zoom,
          initialPosition.y * glm::cos(angle) * zoom, initialPosition.z * zoom};
}

void updateDeltaTime() {
  float currentTime = glfwGetTime();
  deltaTime = currentTime - lastTime;
  lastTime = currentTime;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (action == GLFW_PRESS) {
    // selector movement
    if ((key == GLFW_KEY_UP) && selector.y < 7)
      selector.y++;
    if ((key == GLFW_KEY_LEFT) && selector.x > 0)
      selector.x--;
    if ((key == GLFW_KEY_DOWN) && selector.y > 0)
      selector.y--;
    if ((key == GLFW_KEY_RIGHT) && selector.x < 7)
      selector.x++;

    if (key == GLFW_KEY_T)
      usingAdvancedShaders = !usingAdvancedShaders;

    if (key == GLFW_KEY_ENTER)
      selectorPressed = true;
  }

  int pressableKeys[] = {GLFW_KEY_P, GLFW_KEY_O, GLFW_KEY_H, GLFW_KEY_L,
                         GLFW_KEY_Q}; // keys that is pressable
  for (int pressableKey : pressableKeys) {
    if (key == pressableKey) {
      if (action == GLFW_PRESS)
        pressedKey[pressableKey] = true;
      if (action == GLFW_RELEASE)
        pressedKey[pressableKey] = false;
    }
  }
}
