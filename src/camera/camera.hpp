#pragma once
#include "glm/glm.hpp"

enum struct DIRECTION { FORWARD, BACKWARD, LEFT, RIGHT };

class Camera {
public:
  Camera(float fov, float aspectRatio, float nearPlane, float farPlane);
  glm::mat4 getProjectionMatrix() const;
  glm::mat4 getViewMatrix() const;
  glm::vec3 getDirectionVector();
  glm::vec3 getPosition() const { return position; }
  void ProcessMouse(double xOffset, double yOffset);
  void ProcessKeyboard(DIRECTION mouseDirection, float deltaTime);
  void ProcessScroll(double yOffset);

private:
  const float sensitivity = 0.1f;
  glm::mat4 projection;
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 up;
  float speed;
  float pitch;
  float yaw;
  float fov;
  float aspectRatio;
  float nearPlane;
  float farPlane;

  void recomputeVectors();
};
