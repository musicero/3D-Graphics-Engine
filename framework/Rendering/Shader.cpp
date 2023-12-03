#include "Shader.h"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>

Shader::Shader(const std::string &vertexSrc, const std::string &fragmentSrc,
               const std::string &elementName) {

  GLuint VertexShader = Shader::CompileShader(GL_VERTEX_SHADER, vertexSrc);
  GLuint FragmentShader =
      Shader::CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);

  Shader::CheckForErrors(VertexShader, elementName + " VertexShader");
  Shader::CheckForErrors(FragmentShader, elementName + "FragmentShader");
  ShaderProgram = glCreateProgram();
  glAttachShader(ShaderProgram, VertexShader);
  glAttachShader(ShaderProgram, FragmentShader);
  glLinkProgram(ShaderProgram);

  // Check for shader program linking errors
  GLint success = 0;
  glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    GLchar infoLog[512];
    glGetProgramInfoLog(ShaderProgram, 512, NULL, infoLog);
    std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
  }
  glDeleteShader(VertexShader);
  glDeleteShader(FragmentShader);
  glUseProgram(ShaderProgram);
}

Shader::~Shader() { glDeleteShader(ShaderProgram); }

void Shader::Bind() const { glUseProgram(ShaderProgram); }
void Shader::Unbind() const { glUseProgram(0); }

void Shader::UploadUniformFloat(const std::string &name, const float value) {
  Bind();
  GLuint uniform_location = glGetUniformLocation(ShaderProgram, name.c_str());
  glUniform1f(uniform_location, value);
}

void Shader::UploadUniformFloat2(const std::string &name,
                                 const glm::vec2 &vector) {
  Bind();
  GLuint uniform_location = glGetUniformLocation(ShaderProgram, name.c_str());
  glUniform2f(uniform_location, vector.x, vector.y);
}

void Shader::UploadUniformFloat3(const std::string &name,
                                 const glm::vec3 &vector) {
  Bind();
  GLuint uniform_location = glGetUniformLocation(ShaderProgram, name.c_str());
  glUniform3f(uniform_location, vector.x, vector.y, vector.z);
}

void Shader::UploadUniformInt(const std::string &name, const int value) {
  Bind();
  GLuint uniform_location = glGetUniformLocation(ShaderProgram, name.c_str());
  glUniform1i(uniform_location, value);
}

// Possible problem
void Shader::UploadUniformInt2(const std::string &name,
                               const glm::vec2 &vector) {
  Bind();
  GLuint uniform_location = glGetUniformLocation(ShaderProgram, name.c_str());
  glUniform2i(uniform_location, vector.x, vector.y);
}

void Shader::UploadUniformFloatM4(const std::string &name,
                                  const glm::mat4 &matrix) {
  Bind();
  GLuint uniform_location = glGetUniformLocation(ShaderProgram, name.c_str());
  glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(matrix));
}

GLuint Shader::CompileShader(GLenum shaderType, const std::string &shaderSrc) {
  auto Shader = glCreateShader(shaderType);
  const GLchar *ss = shaderSrc.c_str();
  glShaderSource(Shader, 1, &ss, nullptr);
  glCompileShader(Shader);

  return (Shader);
}

void Shader::CheckForErrors(GLuint Shader, const std::string &shaderName) {
  GLint success = 0;
  glGetShaderiv(Shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar infoLog[512];
    glGetShaderInfoLog(Shader, 512, NULL, infoLog);
    std::cerr << shaderName << " compilation failed:\n" << infoLog << std::endl;
  }
}
