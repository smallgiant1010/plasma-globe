#pragma once
#include "abstract/primitive.hpp"
#include <string>

class Sphere : public Primitive {
public:
  Sphere(const std::string &sphereName, float radius, unsigned int sectorCount,
         unsigned int stackCount);
  ~Sphere() override;
  Sphere(const Sphere &other);
  Sphere& operator=(const Sphere& other);
  void draw() const override;
  const std::string &getName() const { return sphereName; }
  float getRadius() const { return radius; }

private:
  float radius;
  unsigned int sectorCount;
  unsigned int stackCount;
  unsigned int EBO;
  std::string sphereName;
  std::vector<float> normal;
  std::vector<unsigned int> indices;
  void cleanup();
  void setupVectors() override;
  void setupObjects() override;
};
