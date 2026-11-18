#pragma once
#include "abstract/primitive.hpp"
#include <functional>
#include <vector>
#include <glm/glm.hpp>

class Filament : public Primitive {
public:
	struct BezierData {
		glm::vec3 p0;
		glm::vec3 p1;
		glm::vec3 p2;
		glm::vec3 p3;
		unsigned int segments;
	};
  Filament(const std::vector<std::function<float(float)>> &basis, BezierData *bezierData, unsigned int VAO);
  ~Filament() override;
  Filament(const Filament &other);
  Filament& operator=(const Filament&) = delete;
  const BezierData *getBezierData() const { return bezierData; }
  void draw() const override;
private:
  const std::vector<std::function<float(float)>> *basis;
  BezierData *bezierData;
  void setupVectors() override;
  void setupObjects() override;
};
