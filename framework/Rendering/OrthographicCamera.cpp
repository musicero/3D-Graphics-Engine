#include "OrthographicCamera.h"

#include <glm/glm.hpp>

void OrthographicCamera::RecalculateMatrix() {
	// Calculate the orthographic projection matrix
    ProjectionMatrix = glm::ortho(CameraFrustrum.left, CameraFrustrum.right,
                                  CameraFrustrum.bottom, CameraFrustrum.top,
                                  CameraFrustrum.near, CameraFrustrum.far);

    // Calculate the view matrix by applying the camera's position and rotation
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), -Position);

    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    ViewMatrix = rotation * translation;

    // Calculate the view-projection matrix
    ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}
