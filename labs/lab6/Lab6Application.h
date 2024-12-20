#ifndef LAB5APPLICATION_H
#define LAB5APPLICATION_H

#include "GLFWApplication.h"
#include "glad/glad.h"

#include <string>
class Lab6Application : public GLFWApplication {
public:
  const std::string name;
  const std::string version;

  Lab6Application(const std::string &name, const std::string &version);
  GLuint LoadTextures(const std::string &file, GLuint slot);
  GLuint LoadCubeMap(const std::string &file, GLuint slot);

  unsigned Run();
};

#endif
