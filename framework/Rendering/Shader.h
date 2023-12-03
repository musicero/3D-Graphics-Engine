#ifndef SHADERS_H
#define SHADERS_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

#include "glm/ext/matrix_clip_space.hpp"

class Shader {
public:
  Shader(const std::string &vertexSrc, const std::string &fragmentSrc,
         const std::string &elementName);
  ~Shader();

  void Bind() const;
  void Unbind() const;
  void UploadUniformFloat(const std::string &name, const float value);
  void UploadUniformFloat2(const std::string &name, const glm::vec2 &vector);
  void UploadUniformFloat3(const std::string &name, const glm::vec3 &vector);
  void UploadUniformInt(const std::string &name, const int value);
  void UploadUniformInt2(const std::string &name, const glm::vec2 &vector);
  void UploadUniformFloatM4(const std::string &name, const glm::mat4 &matrix);

private:
  GLuint ShaderProgram;

  GLuint CompileShader(GLenum shaderType, const std::string &shaderSrc);
  void CheckForErrors(GLuint Shader, const std::string &shaderName);
};
#endif
