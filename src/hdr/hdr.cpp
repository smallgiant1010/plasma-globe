#include "hdr.hpp"
#include <iostream>

HDRBuffer::HDRBuffer(float exposure, int width, int height) {
  this->exposure = exposure;
  this->screenWidth = width;
  this->screenHeight = height;
  setupObjects();
}

HDRBuffer::~HDRBuffer() { cleanup(); }

HDRBuffer::HDRBuffer(const HDRBuffer &other) {
  this->exposure = other.exposure;
  this->screenWidth = other.screenWidth;
  this->screenHeight = other.screenHeight;
  setupObjects();
}

HDRBuffer &HDRBuffer::operator=(const HDRBuffer &other) {
  if (this != &other) {
    cleanup();
    this->exposure = other.exposure;
    this->screenWidth = other.screenWidth;
    this->screenHeight = other.screenHeight;
    setupObjects();
  }
  return *this;
}

float HDRBuffer::getExposure() const { return exposure; }

void HDRBuffer::bindFBO() const {
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
  glClearColor(0.05f, 0.08f, 0.08f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void HDRBuffer::unbindFBO() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void HDRBuffer::bindCBO(unsigned int pingPongBuffer) const {
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, CBO[0]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, pingPongBuffer);
}

void HDRBuffer::cleanup() {
  glDeleteFramebuffers(1, &FBO);
  glDeleteTextures(2, CBO);
  glDeleteRenderbuffers(1, &DBO);
}

void HDRBuffer::setupObjects() {
  glGenFramebuffers(1, &FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);

  glGenTextures(2, CBO);
  for (int i{}; i < 2; ++i) {
    glBindTexture(GL_TEXTURE_2D, CBO[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0,
                 GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                           GL_TEXTURE_2D, CBO[i], 0);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  glGenRenderbuffers(1, &DBO);
  glBindRenderbuffer(GL_RENDERBUFFER, DBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth,
                        screenHeight);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, DBO);
  unsigned int colorAttachments[2] = {GL_COLOR_ATTACHMENT0,
                                      GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, colorAttachments);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER::INCOMPLETE" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
