#include "filamentSystem.hpp"
#include "glad/glad.h"
#include <cmath>

FilamentSystem::FilamentSystem(float innerContactRadius, float outerContactRadius, float depth, unsigned int segments) {
	this->innerContactRadius = innerContactRadius;
	this->outerContactRadius = outerContactRadius;
  this->depth = depth;
  this->segments = segments;
  setupObjects();
}

FilamentSystem::~FilamentSystem() { cleanup(); }

FilamentSystem::FilamentSystem(const FilamentSystem &other) {
  this->segments = other.segments;
  this->depth = other.depth;
  this->generator = other.generator;
  setupObjects();
  for (auto &detailedFilament : other.detailedFilaments) {
    Filament *filament = detailedFilament.first.get();
    auto &cones = detailedFilament.second;
    std::unique_ptr<Filament> newFilament = std::make_unique<Filament>(
        basis, new Filament::BezierData(*filament->getBezierData()),
        filamentVAO);
    std::unique_ptr<Cone> inner = std::make_unique<Cone>(
        new Cone::ConeData(*cones.first->getConeData()), coneVAO);
    std::unique_ptr<Cone> outer = std::make_unique<Cone>(
        new Cone::ConeData(*cones.second->getConeData()), coneVAO);
    this->detailedFilaments.emplace(
        std::move(newFilament),
        std::make_pair(std::move(inner), std::move(outer)));
  }
}

FilamentSystem &FilamentSystem::operator=(const FilamentSystem &other) {
  if (this != &other) {
    basis.clear();
    detailedFilaments.clear();
    cleanup();
    this->segments = other.segments;
    this->depth = other.depth;
    this->generator = other.generator;
    setupObjects();
    for (auto &detailedFilament : other.detailedFilaments) {
      Filament *filament = detailedFilament.first.get();
      auto &cones = detailedFilament.second;
      std::unique_ptr<Filament> newFilament = std::make_unique<Filament>(
          basis, new Filament::BezierData(*filament->getBezierData()),
          filamentVAO);
      std::unique_ptr<Cone> inner = std::make_unique<Cone>(
          new Cone::ConeData(*cones.first->getConeData()), coneVAO);
      std::unique_ptr<Cone> outer = std::make_unique<Cone>(
          new Cone::ConeData(*cones.second->getConeData()), coneVAO);
      this->detailedFilaments.emplace(
          std::move(newFilament),
          std::make_pair(std::move(inner), std::move(outer)));
    }
  }

  return *this;
}

void FilamentSystem::drawFilaments() const {
  glBindVertexArray(filamentVAO);
  for (auto &detailedFilament : detailedFilaments) {
    detailedFilament.first->draw();
  }

  glBindVertexArray(0);
}

void FilamentSystem::drawContactSpots() const {
  glBindVertexArray(coneVAO);
  for (auto &detailedFilament : detailedFilaments) {
    detailedFilament.second.first->draw();
    detailedFilament.second.second->draw();
  }

  glBindVertexArray(0);
}

void FilamentSystem::createDetailedFilament(float innerRadius,
                                            float outerRadius) {
  std::pair<glm::vec3, glm::vec3> points =
      randomSpherePoints(innerRadius, outerRadius);
  std::unique_ptr<Filament> newFilament =
      addFilament(points.first, points.second);
  std::unique_ptr<Cone> inner =
      addContactSpot(points.first, -glm::normalize(points.first), innerContactRadius);
  std::unique_ptr<Cone> outer =
      addContactSpot(points.second, glm::normalize(points.second), outerContactRadius);
  detailedFilaments.emplace(std::move(newFilament),
                            std::make_pair(std::move(inner), std::move(outer)));
}

void FilamentSystem::destroyDetailedFilament() {
  if (detailedFilaments.empty())
    return;
  detailedFilaments.erase(detailedFilaments.begin());
}

void FilamentSystem::cleanup() {
  glDeleteVertexArrays(1, &filamentVAO);
  glDeleteVertexArrays(1, &coneVAO);
}

std::unique_ptr<Filament> FilamentSystem::addFilament(glm::vec3 p0,
                                                      glm::vec3 p3) {
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
  return std::make_unique<Filament>(basis, data, filamentVAO);
}

std::unique_ptr<Cone> FilamentSystem::addContactSpot(glm::vec3 p, glm::vec3 d,
                                                     float radius) {
  Cone::ConeData *data = new Cone::ConeData{p, d, radius, depth};
  return std::make_unique<Cone>(data, coneVAO);
}

std::pair<glm::vec3, glm::vec3>
FilamentSystem::randomSpherePoints(float innerRadius, float outerRadius) {
  // generate random points on sphere
  const float PI = std::acos(-1.0f);
  float u = randomFloat(-1.0f, 1.0f);
  float phi = randomFloat(0.0f, 2.0f * PI);
  float xy = std::sqrt(1.0f - u * u);
  glm::vec3 p0 = glm::vec3(innerRadius * xy * std::cos(phi),
                           innerRadius * xy * std::sin(phi), innerRadius * u);
  glm::vec3 p3 = p0 * (outerRadius / innerRadius);

	// make the points a little inwards so the cones have space to render
	p0 = glm::normalize(p0) * (innerRadius + depth);
	p3 = glm::normalize(p3) * (outerRadius - depth);

  return {p0, p3};
}

void FilamentSystem::setupObjects() {
  glGenVertexArrays(1, &filamentVAO);
  glBindVertexArray(filamentVAO);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glGenVertexArrays(1, &coneVAO);
  glBindVertexArray(coneVAO);
  glEnableVertexAttribArray(0);
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

  std::function<float(float)> firstDerivativeFunction =
      [](const float t) -> float { return 3 * std::pow(1 - t, 2); };
  basis.push_back(firstDerivativeFunction);

  std::function<float(float)> secondDerivativeFunction =
      [](const float t) -> float { return 6 * (1 - t) * t; };
  basis.push_back(secondDerivativeFunction);

  std::function<float(float)> thirdDerivativeFunction =
      [](const float t) -> float { return 3 * t * t; };
  basis.push_back(thirdDerivativeFunction);
}

float FilamentSystem::randomFloat(float min, float max) {
  std::uniform_real_distribution<float> dist(min, max);
  return dist(generator);
}
