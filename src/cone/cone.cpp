#include "cone.hpp"
#include <cmath>
#include <glad/glad.h>

Cone::Cone(ConeData *coneData, unsigned int VAO) {
  this->coneData = coneData;
  this->VAO = VAO;
  setupVectors();
  setupObjects();
}

Cone::~Cone() {
	cleanup();
}

Cone::Cone(const Cone &other) {
  this->coneData = new ConeData(*other.coneData);
  this->VAO = other.VAO;
  setupVectors();
  setupObjects();
}

Cone &Cone::operator=(const Cone &other) {
  if (this != &other) {
		cleanup();
    this->coneData = new ConeData(*other.coneData);
    this->VAO = other.VAO;

    position.clear();
    indices.clear();

    setupVectors();
    setupObjects();
  }
  return *this;
}

void Cone::draw() const {
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, (void *)0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Cone::cleanup() {
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  delete coneData;
}

void Cone::setupVectors() {
  glm::vec3 dir = glm::normalize(coneData->direction);
  glm::vec3 ref = (std::abs(dir.y) < 0.999f) ? glm::vec3(0.0f, 1.0f, 0.0f)
                                             : glm::vec3(1.0f, 0.0f, 0.0f);
  glm::vec3 u = glm::normalize(glm::cross(ref, dir));
  glm::vec3 v = glm::normalize(glm::cross(dir, u));
  const float PI = std::acos(-1.0f);
  const unsigned int stackCount = 6;
  const unsigned int sectorCount = 16;
  for (unsigned int stackStep{}; stackStep <= stackCount; ++stackStep) {
    float t = static_cast<float>(stackStep) / stackCount;
    float z = coneData->depth * t;
    float r = coneData->radius * t;
    for (unsigned int sectorStep{}; sectorStep <= sectorCount; ++sectorStep) {
      float theta = 2 * PI * sectorStep / sectorCount;
      float x = r * std::cos(theta);
      float y = r * std::sin(theta);
      glm::vec3 point = coneData->endpoint + dir * z + u * x + v * y;

      position.push_back(point.x);
      position.push_back(point.y);
      position.push_back(point.z);
    }
  }

  int k1, k2;
  for (unsigned int i{}; i < stackCount; ++i) {
    k1 = i * (sectorCount + 1);
    k2 = k1 + sectorCount + 1;
    for (unsigned int j{}; j < sectorCount; ++j, ++k1, ++k2) {
      if (i != 0) {
        indices.push_back(k1);
        indices.push_back(k1 + 1);
        indices.push_back(k2);
      }

			indices.push_back(k1 + 1);
			indices.push_back(k2 + 1);
			indices.push_back(k2);
    }
  }
}

void Cone::setupObjects() {
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, position.size() * sizeof(float),
               position.data(), GL_DYNAMIC_DRAW);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
