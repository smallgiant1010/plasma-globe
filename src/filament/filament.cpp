#include "filament.hpp"
#include <glad/glad.h>
#include <functional>

Filament::Filament(const std::vector<std::function<float(float)>> &basis, BezierData *bezierData, unsigned int VAO) {
	this->basis = &basis;
	this->bezierData = bezierData;
	this->VAO = VAO;
	setupVectors();
	setupObjects();
}

Filament::~Filament() {
	glDeleteBuffers(1, &VBO);
	delete bezierData;
}

Filament::Filament(const Filament &other) {
	this->basis = other.basis;
	this->bezierData = new BezierData(*other.bezierData);
	this->VAO = other.VAO;
	setupVectors();
	setupObjects();
}

void Filament::draw() const {
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(3 * sizeof(float)));
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(6 * sizeof(float)));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(position.size() / 7));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Filament::setupVectors() {
	const	glm::vec3 p0 = bezierData->p0;
	const	glm::vec3 p1 = bezierData->p1;
	const	glm::vec3 p2 = bezierData->p2;
	const	glm::vec3 p3 = bezierData->p3;


	auto addVertex = [this](const glm::vec3 &p, const glm::vec3& d, float t) -> void {
		this->position.push_back(p.x);
		this->position.push_back(p.y);
		this->position.push_back(p.z);
		this->position.push_back(d.x);
		this->position.push_back(d.y);
		this->position.push_back(d.z);
		this->position.push_back(t);
	};

	position.clear();

	for(unsigned int i{};i <= bezierData->segments;++i) {
		float t = static_cast<float>(i) / bezierData->segments;
		float b0 = basis->at(0)(t);
		float b1 = basis->at(1)(t);
		float b2 = basis->at(2)(t);
		float b3 = basis->at(3)(t); 
		glm::vec3 point = b0 * p0 + b1 * p1 + b2 * p2 + b3 * p3;
		float d0 = basis->at(4)(t);
		float d1 = basis->at(5)(t);
		float d2 = basis->at(6)(t);
		glm::vec3 tangent = d0 * (p1 - p0) + d1 * (p2 - p1) + d2 * (p3 - p2);
		addVertex(point, tangent, t);
		addVertex(point, tangent, t);
	}
}

void Filament::setupObjects() {
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, position.size() * sizeof(float), position.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
