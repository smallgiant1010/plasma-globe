#include "blur.hpp"
#include <glad/glad.h>
#include <iostream>

Blur::Blur(int amount, int width, int height) {
  this->screenWidth = width;
  this->screenHeight = height;
  this->amount = amount;
  setupObjects();
}

Blur::~Blur() { cleanup(); }

Blur::Blur(const Blur &other) {
  this->screenWidth = other.screenWidth;
  this->screenHeight = other.screenHeight;
  this->amount = other.amount;
  setupObjects();
}

Blur &Blur::operator=(const Blur &other) {
  if (this != &other) {
    cleanup();
    this->screenWidth = other.screenWidth;
    this->screenHeight = other.screenHeight;
    this->amount = other.amount;
    setupObjects();
  }
  return *this;
}

void Blur::cleanup() {
  glDeleteTextures(2, pingPongCBO);
  glDeleteFramebuffers(2, pingPongFBO);
}

void Blur::setupObjects() {
  glGenFramebuffers(2, pingPongFBO);
  glGenTextures(2, pingPongCBO);
  for (int i{}; i < 2; ++i) {
    glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[i]);
    glBindTexture(GL_TEXTURE_2D, pingPongCBO[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0,
                 GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           pingPongCBO[i], 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "ERROR::FRAMEBUFFER::INCOMPLETE" << std::endl;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}
