#pragma once
#include "abstract/primitive.hpp"

class Cube : public Primitive {
public:
	Cube();
	~Cube() override;
	Cube(const Cube& other);
	void draw() const override;
private:
	unsigned int EBO;
	std::vector<float> normal;
	std::vector<unsigned int> indices;
	void setupVectors() override;
	void setupObjects() override;
};
