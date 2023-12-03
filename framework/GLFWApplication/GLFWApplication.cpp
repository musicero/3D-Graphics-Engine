#include "GLFWApplication.h"

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <iostream>

void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

GLFWApplication::GLFWApplication(const std::string &name,
                                 const std::string &version)
    : name(name), version(version) {}

unsigned GLFWApplication::Init() {

  if (!glfwInit()) {
    std::cerr << "ERROR: glfwInit() failed!" << std::endl;
    std::cin.get();
    return EXIT_FAILURE;
  }

  glfwSetErrorCallback(error_callback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  window = glfwCreateWindow(800, 600, "Mywindow", NULL, NULL);
  if (!window) {
    std::cout << "Error: could not create window!" << std::endl;
    return 0;
  }

  return 0;
}
