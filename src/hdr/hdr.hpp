#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

class HDRBuffer {
public:
	HDRBuffer(float exposure, int width, int height);
	~HDRBuffer();
	HDRBuffer(const HDRBuffer& other);
	HDRBuffer& operator=(const HDRBuffer& other) = default;
	float getExposure() const;
	void bindFBO() const;
	void unbindFBO() const;
	void bindCBO() const;

private:
	unsigned int FBO; // framebuffer object
	unsigned int CBO; // color buffer
	unsigned int DBO; // depth buffer
	int screenWidth;
	int screenHeight;
	float exposure;
	void setupObjects();
};
