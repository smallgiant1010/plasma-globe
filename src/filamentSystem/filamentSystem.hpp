#pragma once

#include "cone/cone.hpp"
#include "filament/filament.hpp"
#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <random>
#include <unordered_map>
#include <utility>
#include <vector>

class FilamentSystem {
public:
  FilamentSystem(float innerContactRadius = 0.02f, float outerContactRadius = 0.05f, float depth = 0.03f, unsigned int segments = 32);
  ~FilamentSystem();
  FilamentSystem(const FilamentSystem &other);
  FilamentSystem& operator=(const FilamentSystem &other);
  void drawFilaments() const;
  void drawContactSpots() const;
  void createDetailedFilament(float innerRadius, float outerRadius);
  void destroyDetailedFilament();
  unsigned int getNumberOfFilaments() const { return detailedFilaments.size(); }
private:
  unsigned int filamentVAO;
  unsigned int coneVAO;
  unsigned int segments;
  float depth;
  float innerContactRadius;
  float outerContactRadius;
  std::unordered_map<std::unique_ptr<Filament>,
                     std::pair<std::unique_ptr<Cone>, std::unique_ptr<Cone>>>
      detailedFilaments;
  std::vector<std::function<float(float)>> basis;
  std::mt19937 generator{std::random_device{}()};
  void cleanup();
  void setupObjects();
  float randomFloat(float min, float max);
  std::pair<glm::vec3, glm::vec3> randomSpherePoints(float innerRadius,
                                                     float outerRadius);
	std::unique_ptr<Filament> addFilament(glm::vec3 p0, glm::vec3 p3);
	std::unique_ptr<Cone> addContactSpot(glm::vec3 p, glm::vec3 d, float radius);
};
