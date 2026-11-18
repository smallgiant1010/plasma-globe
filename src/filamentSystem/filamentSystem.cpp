#include "filamentSystem.hpp"
#include "glad/glad.h"
#include <cmath>

FilamentSystem::FilamentSystem(unsigned int segments) {
  this->segments = segments;
  setupObjects();
}

FilamentSystem::~FilamentSystem() {
  glDeleteVertexArrays(1, &VAO);
}

FilamentSystem::FilamentSystem(const FilamentSystem &other) {
  this->segments = other.segments;
  this->generator = other.generator;
  setupObjects();
  for(auto &filament : other.filaments) {
		filaments.push_back(std::make_unique<Filament>(basis, new Filament::BezierData(*filament->getBezierData()), other.VAO));
  }
}

void FilamentSystem::drawFilaments() const {
  glBindVertexArray(VAO);

  for (auto &filament : filaments) {
    filament->draw();
  }

  glBindVertexArray(0);
}

void FilamentSystem::addFilament(float innerRadius, float outerRadius) {
	// generate random points on sphere
	const float PI = std::acos(-1.0f);
	float u = randomFloat(-1.0f, 1.0f);
	float phi = randomFloat(0.0f, 2.0f * PI);
	float xy = std::sqrt(1.0f - u * u);
	glm::vec3 p0 = glm::vec3(innerRadius * xy * std::cos(phi), innerRadius * xy * std::sin(phi), innerRadius * u);
	glm::vec3 p3 = p0 * (outerRadius / innerRadius);

  // imaginary cylinder axis
  glm::vec3 axis = glm::normalize(p3 - p0);
  glm::vec3 reference = std::abs(axis.y) < 0.9f ? glm::vec3(0.0f, 1.0f, 0.0f)
                                                : glm::vec3(1.0f, 0.0f, 0.0f);

  // linear interpolation
  float t1 = randomFloat(0.2f, 0.45f);
  float t2 = randomFloat(0.5f, 0.75f);

  // random rotation around axis
  float offset1 = randomFloat(0.05f, 0.25f);
  float offset2 = randomFloat(0.05f, 0.25f);
  float angle1 = randomFloat(0.0f, 360.0f);
  float angle2 = randomFloat(0.0f, 360.0f);

  // initial points on axis
  glm::vec3 p1 = (1 - t1) * p0 + t1 * p3;
  glm::vec3 p2 = (1 - t2) * p0 + t2 * p3;

  // perpendicular vectors to axis
  glm::vec3 U = glm::normalize(glm::cross(axis, reference));
  glm::vec3 V = glm::normalize(glm::cross(axis, U));

  // points moved away from axis
  p1 += offset1 * (std::cos(glm::radians(angle1)) * U +
                   std::sin(glm::radians(angle1)) * V);
  p2 += offset2 * (std::cos(glm::radians(angle2)) * U +
                   std::sin(glm::radians(angle2)) * V);

  // ensure no point draws out of outer sphere
  if (glm::length(p1) > glm::length(p3)) {
    p1 = glm::normalize(p1) * (glm::length(p3) - 0.01f);
  }
  if (glm::length(p2) > glm::length(p3)) {
    p2 = glm::normalize(p2) * (glm::length(p3) - 0.01f);
  }

  // bezier data
  Filament::BezierData *data =
      new Filament::BezierData{p0, p1, p2, p3, segments};
  filaments.push_back(std::make_unique<Filament>(basis, data, VAO));
}

void FilamentSystem::removeFilament() {
	if(filaments.empty()) return;
  filaments.pop_back();
}

void FilamentSystem::setupObjects() {
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);
  std::function<float(float)> firstFunction = [](const float t) -> float {
    return std::pow(1 - t, 3);
  };
  basis.push_back(firstFunction);

  std::function<float(float)> secondFunction = [](const float t) -> float {
    return 3 * std::pow(1 - t, 2) * t;
  };
  basis.push_back(secondFunction);

  std::function<float(float)> thirdFunction = [](const float t) -> float {
    return 3 * (1 - t) * t * t;
  };
  basis.push_back(thirdFunction);

  std::function<float(float)> fourthFunction = [](const float t) -> float {
    return std::pow(t, 3);
  };
  basis.push_back(fourthFunction);
}

float FilamentSystem::randomFloat(float min, float max){
  std::uniform_real_distribution<float> dist(min, max);
  return dist(generator);
}
