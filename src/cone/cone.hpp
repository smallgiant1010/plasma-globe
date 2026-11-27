#pragma once
#include "abstract/primitive.hpp"
#include <vector>
#include <glm/glm.hpp>

class Cone : public Primitive {
public:
	struct ConeData {
		glm::vec3 endpoint;
		glm::vec3 direction;
		float radius;
		float depth;
	};
	Cone(ConeData *coneData, unsigned int VAO);
	~Cone();
	Cone(const Cone& other);
	Cone& operator=(const Cone& other);
	void draw() const override;
	const ConeData *getConeData() const { return coneData; }
private:
	ConeData *coneData;
	unsigned int EBO;
	std::vector<unsigned int> indices;
	void cleanup();
	void setupVectors() override;
	void setupObjects() override;
};
