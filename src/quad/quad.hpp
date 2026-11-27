#pragma once
#include "abstract/primitive.hpp"

class Quad : public Primitive {
public:
	Quad();
	~Quad() override;
	Quad(const Quad& other);
	Quad& operator=(const Quad& other);
	void draw() const override;
private:
	unsigned int EBO;
	std::vector<float> texCoords;
	std::vector<unsigned int> indices;
	void cleanup();
	void setupVectors() override;
	void setupObjects() override;
};
