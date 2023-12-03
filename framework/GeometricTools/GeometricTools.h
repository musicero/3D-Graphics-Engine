#ifndef GEOMETRICTOOLS_H
#define GEOMETRICTOOLS_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <array>
#include <vector>

namespace GeometricTools {
struct unitShape {
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;
};
extern unitShape UnitTriangle();
extern unitShape UnitSquare();
extern unitShape UnitGrid(unsigned partitions);
extern unitShape UnitCube();
extern unitShape UnitCubeWNormals();
} // namespace GeometricTools

#endif
