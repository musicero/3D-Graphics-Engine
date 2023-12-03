#include "PerspectiveCamera.h"

#include "glm/gtx/string_cast.hpp"
#include <glm/glm.hpp>
#include <iomanip>
#include <iostream>

PerspectiveCamera::PerspectiveCamera(const Frustrum &frustrum,
                                     const glm::vec3 &position,
                                     const glm::vec3 &lookAt,
                                     const glm::vec3 &upVector) {
  this->CameraFrustrum = frustrum;
  this->Position = position;
  this->LookAt = lookAt;
  this->UpVector = upVector;
  PerspectiveCamera::RecalculateMatrix();
}

void PerspectiveCamera::RecalculateMatrix() {
  this->ProjectionMatrix = glm::perspective(
      glm::radians(CameraFrustrum.angle),           // FOV
      CameraFrustrum.width / CameraFrustrum.height, // Aspect Ratio
      CameraFrustrum.near,                          // Near clipping plane
      CameraFrustrum.far                            // Far clipping plane
  );

  this->ViewMatrix = glm::lookAt(Position, LookAt, UpVector);
  this->ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

void PerspectiveCamera::PrintAttributes() {
  std::cout << "DEBUGGING CAMERA: " << std::endl;
  std::cout << "POSITION:" << glm::to_string(this->Position) << std::endl;
  std::cout << "UPVECTOR: " << glm::to_string(this->UpVector) << std::endl;
  std::cout << "LOOKAT:   " << glm::to_string(this->LookAt) << std::endl;

  std::cout << "View Matrix: " << std::endl;
  printMatrix(this->ViewMatrix);
  std::cout << "Projection Matrix" << std::endl;
  printMatrix(this->ProjectionMatrix);
  std::cout << "View-Projection Matrix" << std::endl;
  printMatrix(this->ViewProjectionMatrix);
}

void PerspectiveCamera::printMatrix(glm::mat4 &matrix) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      std::cout << std::setw(9) << matrix[i][j] << " ";
    }
    std::cout << std::endl;
  }
}
void PerspectiveCamera::printMatrix(glm::mat3 &matrix) {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      std::cout << std::setw(9) << matrix[i][j] << " ";
    }
    std::cout << std::endl;
  }
}
