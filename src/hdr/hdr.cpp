#include "hdr.hpp"
#include <iostream>

HDRBuffer::HDRBuffer(float exposure, int width, int height) {
	this->exposure = exposure;
	this->screenWidth = width;
	this->screenHeight = height;
	setupObjects();
}

HDRBuffer::~HDRBuffer() {
	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &CBO);
	glDeleteRenderbuffers(1, &DBO);
}

HDRBuffer::HDRBuffer(const HDRBuffer& other) {
	this->exposure = other.exposure;
	this->screenWidth = other.screenWidth;
	this->screenHeight = other.screenHeight;
	setupObjects();
}

float HDRBuffer::getExposure() const {
	return exposure;
}

void HDRBuffer::bindFBO() const {
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void HDRBuffer::unbindFBO() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HDRBuffer::bindCBO() const {
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, CBO);
}

void HDRBuffer::setupObjects() {
	glGenFramebuffers(1, &FBO);

	glGenTextures(1, &CBO);
	glBindTexture(GL_TEXTURE_2D, CBO);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenRenderbuffers(1, &DBO);
	glBindRenderbuffer(GL_RENDERBUFFER, DBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, CBO, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DBO);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER::INCOMPLETE" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

