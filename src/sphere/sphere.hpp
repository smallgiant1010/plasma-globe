#pragma once
#include <vector>

class Sphere {
public:
	Sphere(float radius, unsigned int sectorCount, unsigned int stackCount);
	~Sphere();
	Sphere(const Sphere& other);
	void draw() const;
private:
	unsigned int radius;
	unsigned int sectorCount;
	unsigned int stackCount;
	unsigned int VAO, VBO, EBO;
	std::vector<float> position;
	std::vector<float> normal;
	std::vector<unsigned int> indices;
	void setupVectors();
	void setupObjects();
};

