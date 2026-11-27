#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

class HDRBuffer {
public:
	HDRBuffer(float exposure, int width, int height);
	~HDRBuffer();
	HDRBuffer(const HDRBuffer& other);
	HDRBuffer& operator=(const HDRBuffer& other);
	float getExposure() const;
	void bindFBO() const;
	void unbindFBO() const;
	void bindCBO(unsigned int pingPongBuffer) const;
	const unsigned int* getCBO() const { return CBO; }

private:
	unsigned int FBO; // framebuffer object
	unsigned int CBO[2]; // first color buffer for frag color
										 // second color buffer for bright color
	unsigned int DBO; // depth buffer
	int screenWidth;
	int screenHeight;
	float exposure;
	void cleanup();
	void setupObjects();
};
