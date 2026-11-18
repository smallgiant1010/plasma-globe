#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>

#include "camera/camera.hpp"
#include "shader/shader.hpp"
#include "sphere/sphere.hpp"
#include "filamentSystem/filamentSystem.hpp"

#define MAX_WIDTH 800
#define MAX_HEIGHT 600

// paths
std::string innerSphereVertexShader = "assets/inner_sphere_glsl/sphere.vert.glsl";
std::string innerSphereFragmentShader = "assets/inner_sphere_glsl/sphere.frag.glsl";
std::string outerSphereVertexShader = "assets/outer_sphere_glsl/sphere.vert.glsl";
std::string outerSphereFragmentShader = "assets/outer_sphere_glsl/sphere.frag.glsl";
std::string filamentVertexShader = "assets/filament_glsl/filament.vert.glsl";
std::string filamentFragmentShader = "assets/filament_glsl/filament.frag.glsl";

// configurable properties
glm::vec3 glassColor(0.61f, 0.66f, 0.71f);
glm::vec3 lightDirection(-1.0f, -1.0f, -1.0f);
glm::vec3 innerSphereColor(0.0f);
glm::vec3 baseReflectivity(0.04f);
float shininess = 64.0f;
const float PI = std::acos(-1.0f);

// global states
float deltaTime{};
float lastFrame{};
float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouseMovement = true;

// global objects
Camera camera(45.0f, (float)MAX_WIDTH / (float)MAX_HEIGHT, 0.1f, 100.0f);

// glfw related
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *, int width, int height);
void mouse_callback(GLFWwindow *, double xpos, double ypos);
void scroll_callback(GLFWwindow *, double, double yOffset);

// helper functions
void renderObject(Shader *shader, Primitive *primitive);
void renderFilaments(Shader *shader, FilamentSystem *filamentSystem);
void setupShaderMatrices(Shader *shader);

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
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return EXIT_FAILURE;
  }

  {
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

    Shader outerSphereShader(outerSphereVertexShader, outerSphereFragmentShader);
    outerSphereShader.use();

    Shader innerSphereShader(innerSphereVertexShader, innerSphereFragmentShader);
    innerSphereShader.use();

    Shader filamentShader(filamentVertexShader, filamentFragmentShader);
    filamentShader.use();

    Sphere outerGlass("outer", 1.0f, 48, 24);
    Sphere innerElectrode("inner", 0.2f, 48, 24);
    FilamentSystem filamentSystem;
    for(int i{};i < 100;++i) {
			filamentSystem.addFilament(innerElectrode.getRadius(), outerGlass.getRadius());
    }

    while (!glfwWindowShouldClose(window)) {
      float currentFrame = glfwGetTime();
      deltaTime = currentFrame - lastFrame;
      lastFrame = currentFrame;
      processInput(window);

			// restore depth writing before clear
      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);
      
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      renderObject(&innerSphereShader, &innerElectrode);

      renderFilaments(&filamentShader, &filamentSystem);

			glEnable(GL_BLEND);
			glDepthMask(GL_FALSE);
      renderObject(&outerSphereShader, &outerGlass);

      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  }

	glfwDestroyWindow(window);
  glfwTerminate();
  return EXIT_SUCCESS;
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(DIRECTION::FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(DIRECTION::BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(DIRECTION::RIGHT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(DIRECTION::LEFT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow *, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *, double xpos, double ypos) {
  if (firstMouseMovement) {
    lastX = xpos;
    lastY = ypos;
    firstMouseMovement = false;
  }

  float xOffset = xpos - lastX;
  float yOffset = ypos - lastY;
  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouse(xOffset, yOffset);
}

void scroll_callback(GLFWwindow *, double, double yOffset) {
  camera.ProcessScroll(yOffset);
}

void renderObject(Shader *shader, Primitive *primitive) {
	setupShaderMatrices(shader);
  Sphere *sphere = dynamic_cast<Sphere *>(primitive);
  if (sphere) {
    if (sphere->getName() == "outer") {
      shader->setVec3("material.ambient", glassColor);
      shader->setVec3("material.diffuse", glassColor);
      shader->setVec3("material.specular", glm::vec3(1.0f));
      shader->setFloat("material.shininess", shininess);
			shader->setVec3("light.ambient", glm::vec3(0.1f));
			shader->setVec3("light.diffuse", glm::vec3(0.8f));
			shader->setVec3("light.specular", glm::vec3(1.0f));
			shader->setVec3("light.direction", lightDirection);
			shader->setVec3("viewPos", camera.getPosition());
			shader->setVec3("F0", baseReflectivity);
    } else if (sphere->getName() == "inner") {
    	shader->setVec3("objectColor", innerSphereColor);
    }
  }
  primitive->draw();
}

void renderFilaments(Shader *shader, FilamentSystem *filamentSystem) {
	setupShaderMatrices(shader);
	shader->setFloat("PI", PI);
	filamentSystem->drawFilaments();
}

void setupShaderMatrices(Shader *shader) {
	shader->use();
  glm::mat4 model(1.0f);
  shader->setMat4("model", model);
  shader->setMat4("view", camera.getViewMatrix());
  shader->setMat4("projection", camera.getProjectionMatrix());
}
