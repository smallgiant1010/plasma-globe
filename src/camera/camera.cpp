#include "camera/camera.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <cmath>

Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane) {
  projection = glm::mat4(
      glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane));
  speed = 2.5f;
  pitch = 0.0f;
  yaw = -90.0f;
  this->fov = fov;
  this->aspectRatio = aspectRatio;
  this->nearPlane = nearPlane;
  this->farPlane = farPlane;
  position = glm::vec3(0.0f, 0.0f, 3.0f);
  front = glm::vec3(0.0f, 0.0f, -1.0f);
  recomputeVectors();
}

glm::mat4 Camera::getProjectionMatrix() const { 
	return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane); 
}

glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(position, position + front, up);
}

void Camera::ProcessMouse(double xOffset, double yOffset) {
  xOffset *= sensitivity;
  yOffset *= sensitivity;

  yaw += xOffset;
  pitch -= yOffset;
	pitch = std::min(pitch, 89.0f);
	pitch = std::max(pitch, -89.0f);

	recomputeVectors();
}

void Camera::ProcessKeyboard(DIRECTION mouseDirection, float deltaTime) {
  float movementSpeed = speed * deltaTime;
  if (mouseDirection == DIRECTION::FORWARD) {
    position += movementSpeed * front;
  }
  if (mouseDirection == DIRECTION::BACKWARD) {
    position -= movementSpeed * front;
  }
  if (mouseDirection == DIRECTION::RIGHT) {
    position += movementSpeed * glm::normalize(glm::cross(front, up));
  }
  if (mouseDirection == DIRECTION::LEFT) {
    position -= movementSpeed * glm::normalize(glm::cross(front, up));
  }
}

void Camera::ProcessScroll(double yOffset) {
	fov -= (float)yOffset;
	fov = std::max(fov, 1.0f);
	fov = std::min(fov, 100.0f);
}

void Camera::recomputeVectors() {
  glm::vec3 direction;
  direction.x = std::cosf(glm::radians(yaw)) * std::cosf(glm::radians(pitch));
  direction.z = std::sinf(glm::radians(yaw)) * std::cosf(glm::radians(pitch));
  direction.y = std::sinf(glm::radians(pitch));
  front = glm::normalize(direction);
	glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
	up = glm::normalize(glm::cross(right, front));
}







