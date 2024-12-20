#include "GeometricTools.h"
#include <vector>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace GeometricTools {

unitShape UnitTriangle() {
  unitShape shape;
  shape.vertices = {
      /*
      |position 			|texture  	*/
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.0f,
      1.0f,  0.0f,  0.0f, 0.5f, 0.0f, 0.5f, 1.0f,
  };

  shape.indices = {0, 1, 2};

  return shape;
}

unitShape UnitSquare() {
  unitShape shape;
  shape.vertices = {
      /*
  |position           |texture  */
      -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
  };

  shape.indices = {0, 1, 2, 2, 3, 0};

  return shape;
}

unitShape UnitGrid(GLuint partitions) {

  unitShape shape;
  // ==== making the vectors ====

  partitions = static_cast<GLfloat>(partitions);

  GLfloat squareSize = 1.f / partitions;
  for (int i = 0; i <= partitions; i++) {
    for (int j = 0; j <= partitions; j++) {
      // position coordinates
      shape.vertices.push_back(-0.5f + i * squareSize); // x coordinate
      shape.vertices.push_back(-0.5f + j * squareSize); // y coordinate
      shape.vertices.push_back(0.f);                    // z coordinate

      // texture coordinates
      shape.vertices.push_back(i * squareSize);
      shape.vertices.push_back(j * squareSize);

      shape.vertices.push_back(0.0f);
      shape.vertices.push_back(0.0f);
      shape.vertices.push_back(1.0f);
    }
  }

  // ==== making the topology ====

  for (GLuint i = 0; i < partitions; i++) {
    for (GLuint j = 0; j < partitions; j++) {
      GLuint currentVertex = i * (partitions + 1) + j;

      shape.indices.push_back(currentVertex);
      shape.indices.push_back(currentVertex + 1);
      shape.indices.push_back(currentVertex + 1 + (partitions + 1));

      shape.indices.push_back(currentVertex);
      shape.indices.push_back(currentVertex + (partitions + 1));
      shape.indices.push_back(currentVertex + 1 + (partitions + 1));
    }
  }

  return shape;
}

unitShape UnitCube() {
  unitShape shape;
  shape.vertices = {
      /*
       x      y      z     tx    ty  */
      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, //   0-----1 Front
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, //   |     |
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, //   |     |
      0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, //   2-----3

      -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, //   4-----5 Back
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, //   |     |
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, //   |     |
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, //   6-----7
  };

  shape.indices = {
      0, 1, 2, 2, 1, 3, // Front
      4, 5, 6, 6, 5, 7, // Back
      0, 1, 4, 4, 1, 5, // Right
      2, 3, 6, 6, 3, 7, // Left
      0, 2, 4, 4, 2, 6, // Top
      1, 3, 5, 5, 3, 7  // Bottom
  };
  return shape;
}

unitShape UnitCubeWNormals() {
  unitShape shape;
  shape.vertices = {
      /*
       x      y      z      tx    ty    nx   ny   nz      */
      -0.5f, 0.5f,  0.5f,  0.f, 1.f, -1.f, 0.f,  0.f, //   0-----1 Left
      -0.5f, 0.5f,  -0.5f, 1.f, 1.f, -1.f, 0.f,  0.f, //   |     |
      -0.5f, -0.5f, 0.5f,  0.f, 0.f, -1.f, 0.f,  0.f, //   |     |
      -0.5f, -0.5f, -0.5f, 1.f, 0.f, -1.f, 0.f,  0.f, //   2-----3

      0.5f,  0.5f,  -0.5f, 0.f, 1.f, 1.f,  0.f,  0.f, //   4-----5 Right
      0.5f,  0.5f,  0.5f,  1.f, 1.f, 1.f,  0.f,  0.f, //   |     |
      0.5f,  -0.5f, -0.5f, 0.f, 0.f, 1.f,  0.f,  0.f, //   |     |
      0.5f,  -0.5f, 0.5f,  1.f, 0.f, 1.f,  0.f,  0.f, //   6-----7

      -0.5f, 0.5f,  -0.5f, 0.f, 1.f, 0.f,  0.f,  -1.f, //   8-----9 Front
      0.5f,  0.5f,  -0.5f, 1.f, 1.f, 0.f,  0.f,  -1.f, //   |     |
      -0.5f, -0.5f, -0.5f, 0.f, 0.f, 0.f,  0.f,  -1.f, //   |     |
      0.5f,  -0.5f, -0.5f, 1.f, 0.f, 0.f,  0.f,  -1.f, //   10---11

      -0.5f, 0.5f,  0.5f,  0.f, 1.f, 0.f,  0.f,  1.f, //   12---13 Back
      0.5f,  0.5f,  0.5f,  1.f, 1.f, 0.f,  0.f,  1.f, //   |     |
      -0.5f, -0.5f, 0.5f,  0.f, 0.f, 0.f,  0.f,  1.f, //   |     |
      0.5f,  -0.5f, 0.5f,  1.f, 0.f, 0.f,  0.f,  1.f, //   14---15

      -0.5f, -0.5f, -0.5f, 0.f, 1.f, 0.f,  -1.f, 0.f, //   16---17 Bottom
      0.5f,  -0.5f, -0.5f, 1.f, 1.f, 0.f,  -1.f, 0.f, //   |     |
      -0.5f, -0.5f, 0.5f,  0.f, 0.f, 0.f,  -1.f, 0.f, //   |     |
      0.5f,  -0.5f, 0.5f,  1.f, 0.f, 0.f,  -1.f, 0.f, //   18---19

      -0.5f, 0.5f,  0.5f,  0.f, 1.f, 0.f,  1.f,  0.f, //   20---21 Top
      0.5f,  0.5f,  0.5f,  1.f, 1.f, 0.f,  1.f,  0.f, //   |     |
      -0.5f, 0.5f,  -0.5f, 0.f, 0.f, 0.f,  1.f,  0.f, //   |     |
      0.5f,  0.5f,  -0.5f, 1.f, 0.f, 0.f,  1.f,  0.f  //   22---23
  };
  shape.indices = {0,  1,  2,  2,  1,  3,  4,  5,  6,  6,  5,  7,
                   8,  9,  10, 10, 9,  11, 12, 13, 14, 14, 13, 15,
                   16, 17, 18, 18, 17, 19, 20, 21, 22, 22, 21, 23};

  return shape;
}

} // namespace GeometricTools
