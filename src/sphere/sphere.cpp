#include "sphere.hpp"
#include <cmath>
#include "glad/glad.h"

Sphere::Sphere(float radius, unsigned int sectorCount, unsigned int stackCount) {
	this->radius = radius;
	this->sectorCount = sectorCount;
	this->stackCount = stackCount;
	setupVectors();
	setupObjects();
}

Sphere::~Sphere() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

Sphere::Sphere(const Sphere& other) {
	this->radius = other.radius;
	this->sectorCount = other.sectorCount;
	this->stackCount = other.stackCount;
	this->position = other.position;
	this->normal = other.normal;
	this->indices = other.indices;
	setupObjects();
}

void Sphere::draw() const {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void *)0);
	glBindVertexArray(0);
}

void Sphere::setupVectors() {
	float PI = acosf(-1.0); // cosine of 180 degrees is PI
	float TWO_PI = 2 * PI;
	float HALF_PI = PI / 2;
	float length = 1.0f / radius;
	float sectorStep = TWO_PI / sectorCount;
	float stackStep = PI / stackCount;
	for(unsigned int i{};i <= stackCount;++i) {
		float phi = HALF_PI - i * stackStep;
		float xy = radius * cosf(phi);
		float z = radius * sinf(phi);
		for(unsigned int j{};j <= sectorCount;++j) {
			float theta = j * sectorStep;
			float x = xy * cosf(theta);	
			float y = xy * sinf(theta);	
			position.push_back(x);
			position.push_back(y);
			position.push_back(z);

			// normalization via dividing by length
			float nx = x * length;
			float ny = y * length;
			float nz = z * length;
			normal.push_back(nx);
			normal.push_back(ny);
			normal.push_back(nz);
		}
	}

	int k1, k2;
	for(unsigned int i{};i < stackCount;++i) {
		k1 = i * (sectorCount + 1);
		k2 = k1 + sectorCount + 1;
		for(unsigned int j{};j < sectorCount;++j, ++k1, ++k2) {
			if(i != 0) {
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			if(i != (stackCount - 1)) {
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}
}

void Sphere::setupObjects() {
	std::vector<float> data;
	for(unsigned int i{}, j{};i < position.size() && j < normal.size();i += 3,j += 3) {
		data.push_back(position[i]);
		data.push_back(position[i + 1]);
		data.push_back(position[i + 2]);
		data.push_back(normal[j]);
		data.push_back(normal[j + 1]);
		data.push_back(normal[j + 2]);
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW); 

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}














