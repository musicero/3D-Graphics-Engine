#ifndef CUBE_H
#define CUBE_H

#include "Shader.h"
#include "VertexArray.h"
#include "glm/gtc/matrix_transform.hpp"

enum Team { Blue, Red };

class Cube {
private:
  std::shared_ptr<VertexArray> vertexArray;
  std::shared_ptr<Shader> shader;
  glm::mat4 scale;
  glm::mat4 rotate;
  glm::mat4 translate;
  glm::mat4 modelMatrix;
  glm::vec3 color;

public:
  Team team;
  bool selected;
  glm::ivec2 coords;
  bool advancedShaders;
  float globalScale;

private:
  void RecalculateModelMatrix() {
    auto globalScaleMatrix =
        glm::scale(glm::mat4(1.f), {globalScale, globalScale, globalScale});
    modelMatrix = globalScaleMatrix * translate * rotate * scale;
    shader->Bind(); // Unsure if binding is done via UploadUniform...
    shader->UploadUniformFloatM4("u_Model", modelMatrix);
  }

public:
  Cube(std::shared_ptr<VertexArray> &vertexArray_,
       std::shared_ptr<Shader> &shader_)
      : vertexArray(vertexArray_), shader(shader_) {
    scale = glm::scale(glm::mat4(1.f), {1.f, 1.f, 1.f});
    rotate = glm::rotate(glm::mat4(1.f), glm::radians(0.0f), {0.f, 0.f, 1.f});
    translate = glm::translate(glm::mat4(1.f), {0.0f, 0.0f, 0.0f});
    RecalculateModelMatrix();
    color = glm::vec3(0.8f, 0.8f, 0.8f);
    coords = glm::ivec2(0, 0);
    selected = false;
    advancedShaders = true;
    globalScale = 3.f;
  }
  std::shared_ptr<VertexArray> GetVertexArray() { return vertexArray; }
  std::shared_ptr<Shader> GetShader() { return shader; }
  void SetScale(glm::mat4 scale_) {
    scale = scale_;
    RecalculateModelMatrix();
  };
  void SetRotate(glm::mat4 rotate_) {
    rotate = rotate_;
    shader->Bind(); // Unsure if binding is done via UploadUniform...
    shader->UploadUniformFloatM4("u_Rotation", rotate);
    RecalculateModelMatrix();
  };
  void SetTranslate(glm::mat4 translate_) {
    translate = translate_;
    RecalculateModelMatrix();
  };
  void SetColor(glm::vec3 color_) { color = color_; };
  glm::vec3 GetColor() { return color; };
  void setViewProjection(glm::mat4 &viewProjection) {
    shader->UploadUniformFloatM4("u_ViewProjection", viewProjection);
  }
  void Bind() {
    vertexArray->Bind();
    shader->Bind();
  }
  void SetUniforms(const glm::mat4 &viewProjection) {
    shader->Bind();
    shader->UploadUniformFloatM4("u_ViewProjection", viewProjection);
    shader->UploadUniformFloatM4("u_Model", modelMatrix);
    shader->UploadUniformFloatM4("u_Rotation", rotate);
    shader->UploadUniformFloat3("u_baseColor", color);
    shader->UploadUniformInt("u_usingAdvancedShaders", advancedShaders);
  }
  void CalculatePosition(int boardResolutionInt) {
    float boardResolution = static_cast<float>(boardResolutionInt);

    this->SetScale(
        glm::scale(glm::mat4(1.f), glm::vec3(1 / (boardResolution + 1))));

    float xCoord =
        ((this->coords.x / (boardResolution)) + (0.5f / boardResolution)) -
        0.5f;
    float yCoord =
        ((this->coords.y / (boardResolution)) + (0.5f / boardResolution)) -
        0.5f;

    this->SetTranslate(glm::translate(glm::mat4(1.f), {xCoord, yCoord, 0.08f}));
  }
};
#endif
