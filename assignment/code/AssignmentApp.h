#ifndef ASSIGNMENTAPP_H
#define ASSIGNMENTAPP_H

#include "GLFWApplication.h"
#include "glad/glad.h"

#include <string>
class AssignmentApp : public GLFWApplication {
public:
  const std::string name;
  const std::string version;

  AssignmentApp(const std::string &name, const std::string &version);
  GLuint LoadTextures(const std::string &file, GLuint slot);
  GLuint LoadCubeMap(const std::string &file, GLuint slot);

  unsigned Run();
};

#endif
