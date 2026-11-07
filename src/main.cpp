#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "shader/shader.hpp"
#include "sphere/sphere.hpp"

#define MAX_WIDTH 800
#define MAX_HEIGHT 600

std::string sphereVertexShader = "assets/sphere_glsl/sphere.vert.glsl";
std::string sphereFragmentShader = "assets/sphere_glsl/sphere.frag.glsl";

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *_, int width, int height);

int main(void) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow *window =
      glfwCreateWindow(MAX_WIDTH, MAX_HEIGHT, "Plasma Globe", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return EXIT_FAILURE;
  }

  glEnable(GL_DEPTH_TEST);
  
  Shader sphereShader(sphereVertexShader, sphereFragmentShader);
  sphereShader.use();

  Sphere outerGlass(1.0f, 48, 24);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		sphereShader.use();
		glm::mat4 model = glm::mat4(1.0f);
		sphereShader.setMat4("model", model);
		
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		sphereShader.setMat4("view", view);

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)MAX_WIDTH / (float)MAX_HEIGHT, 0.1f, 100.0f);
		sphereShader.setMat4("projection", projection);

		outerGlass.draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return EXIT_SUCCESS;
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *_, int width, int height) {
  glViewport(0, 0, width, height);
}
