#include "cube/cube.hpp"
#include "glad/glad.h"

Cube::Cube() {
  setupVectors();
  setupObjects();
}

Cube::~Cube() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

Cube::Cube(const Cube &other) {
  this->position = other.position;
  this->indices = other.indices;
  setupObjects();
}

void Cube::draw() const {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void *)0);
  glBindVertexArray(0);
}

void Cube::setupVectors() {
  for (int x = -1; x <= 1; x += 2) {
    for (int y = -1; y <= 1; y += 2) {
      for (int z = -1; z <= 1; z += 2) {
        position.push_back(0.5f * x);
        position.push_back(0.5f * y);
        position.push_back(0.5f * z);
      }
    }
  }

  indices = {0, 1, 2, 1, 3, 2,

             4, 6, 5, 6, 7, 5,

             0, 4, 1, 1, 4, 5,

             2, 3, 6, 3, 7, 6,

             0, 4, 2, 2, 6, 4,

             1, 5, 3, 3, 5, 7};
}

void Cube::setupObjects() {
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, position.size() * sizeof(float),
               position.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
