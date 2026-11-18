#pragma once
#include <vector>

class Primitive {
public:
	virtual ~Primitive() = default;
	virtual void draw() const = 0;
protected:
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	std::vector<float> position;
	virtual void setupVectors() = 0;
	virtual void setupObjects() = 0;
};
