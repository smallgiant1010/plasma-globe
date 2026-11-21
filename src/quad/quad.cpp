#include "quad/quad.hpp"
#include "glad/glad.h"

Quad::Quad() {
  setupVectors();
  setupObjects();
}

Quad::~Quad() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

Quad::Quad(const Quad &other) {
  this->position = other.position;
  this->texCoords = other.texCoords;
  this->indices = other.indices;
  setupObjects();
}

void Quad::draw() const {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void *)0);
  glBindVertexArray(0);
}

void Quad::setupVectors() {
	// these are in ndc so they span [-1, 1]
	position = {
		1.0f, 1.0f, 0.0f, // top right
		1.0f, -1.0f, 0.0f, // bottom right
		-1.0f, -1.0f, 0.0f, // bottom left
		-1.0f, 1.0f, 0.0f // top left
	};

	texCoords = {
		1.0f, 1.0f, // top right
		1.0f, 0.0f, // bottom right
		0.0f, 0.0f, // bottom left
		0.0f, 1.0f // top left
	};

	indices = {
		0, 3, 1,
		1, 3, 2
	};
}

void Quad::setupObjects() {
	std::vector<float> data;
	for(unsigned int i{}, j{};i < position.size() && j < texCoords.size();i += 3, j += 2) {
		data.push_back(position[i]);
		data.push_back(position[i + 1]);
		data.push_back(position[i + 2]);

		data.push_back(texCoords[j]);
		data.push_back(texCoords[j + 1]);
	}

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float),
               data.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
