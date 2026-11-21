#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

#include "camera/camera.hpp"
#include "filamentSystem/filamentSystem.hpp"
#include "shader/shader.hpp"
#include "sphere/sphere.hpp"
#include "quad/quad.hpp"
#include "hdr/hdr.hpp"

// macros/aliases
#define MAX_WIDTH 800
#define MAX_HEIGHT 600
using Clock = std::chrono::steady_clock;

// paths
std::string innerSphereVertexShader =
    "assets/inner_sphere_glsl/sphere.vert.glsl";
std::string innerSphereFragmentShader =
    "assets/inner_sphere_glsl/sphere.frag.glsl";
std::string outerSphereVertexShader =
    "assets/outer_sphere_glsl/sphere.vert.glsl";
std::string outerSphereFragmentShader =
    "assets/outer_sphere_glsl/sphere.frag.glsl";
std::string filamentVertexShader = "assets/filament_glsl/filament.vert.glsl";
std::string filamentFragmentShader = "assets/filament_glsl/filament.frag.glsl";
std::string hdrVertexShader = "assets/hdr_glsl/hdr.vert.glsl";
std::string hdrFragmentShader = "assets/hdr_glsl/hdr.frag.glsl";

// configurable properties
glm::vec3 glassColor(0.61f, 0.66f, 0.71f);
glm::vec3 lightDirection(-1.0f, -1.0f, -1.0f);
glm::vec3 innerSphereColor(0.0f);
glm::vec3 baseReflectivity(0.04f);
constexpr float shininess = 64.0f;
const float PI = std::acos(-1.0f);
constexpr float width = 0.005f;

// global states
float deltaTime{};
float lastFrame{};
float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouseMovement = true;
bool isLocked = false;
bool wasPressed = false;
float geometryGenerationTime{};
float cpuDrawTime{};
float gpuDrawTime{};
float lastBenchmarkUpdate{};
constexpr float INTERVAL = 0.5f;
double cpuSum{};
double gpuSum{};
unsigned int sampleCount{};

// global objects
Camera camera(45.0f, (float)MAX_WIDTH / (float)MAX_HEIGHT, 0.1f, 100.0f);
HDRBuffer *hdrBuffer;

// glfw related
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *, double, double yOffset);

// helper functions
void renderObject(Shader *shader, Primitive *primitive);
void renderFilaments(Shader *shader, FilamentSystem *filamentSystem);
void setupShaderMatrices(Shader *shader);
void drawImgui();

int main(void) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow *window =
      glfwCreateWindow(MAX_WIDTH, MAX_HEIGHT, "Plasma Globe", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return EXIT_FAILURE;
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();

  {
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

		hdrBuffer = new HDRBuffer(1.0f, MAX_WIDTH, MAX_HEIGHT);

    Shader outerSphereShader(outerSphereVertexShader,
                             outerSphereFragmentShader);
    outerSphereShader.use();

    Shader innerSphereShader(innerSphereVertexShader,
                             innerSphereFragmentShader);
    innerSphereShader.use();

    Shader filamentShader(filamentVertexShader, filamentFragmentShader);
    filamentShader.use();

		Shader hdrShader(hdrVertexShader, hdrFragmentShader);
		hdrShader.use();
		hdrShader.setInt("hdrBuffer", 0);

    Sphere outerGlass("outer", 1.0f, 48, 24);
    Sphere innerElectrode("inner", 0.2f, 48, 24);
    Quad screenQuad;

    auto start = Clock::now();
    FilamentSystem filamentSystem;
    for (int i{}; i < 200; ++i) {
      filamentSystem.addFilament(innerElectrode.getRadius(),
                                 outerGlass.getRadius());
    }
    auto end = Clock::now();
    geometryGenerationTime =
        std::chrono::duration<float, std::milli>(end - start).count();

    GLuint timerQuery{};
    GLuint64 elapsedNanoSeconds{};
    glGenQueries(1, &timerQuery);

    while (!glfwWindowShouldClose(window)) {
      float currentFrame = glfwGetTime();
      deltaTime = currentFrame - lastFrame;
      lastFrame = currentFrame;

      if(currentFrame - lastBenchmarkUpdate >= INTERVAL) {	
      	if(sampleCount > 0) {
					cpuDrawTime = cpuSum / sampleCount;
					gpuDrawTime = gpuSum / sampleCount;
				}
				sampleCount = 0;
				cpuSum = gpuSum = 0.0;
				lastBenchmarkUpdate = currentFrame;
      }
      processInput(window);

      hdrBuffer->bindFBO();

      renderObject(&innerSphereShader, &innerElectrode);

      glBeginQuery(GL_TIME_ELAPSED, timerQuery);
      start = Clock::now();
      renderFilaments(&filamentShader, &filamentSystem);
      end = Clock::now();
      cpuSum += std::chrono::duration<double, std::milli>(end - start).count();
      glEndQuery(GL_TIME_ELAPSED);
      glGetQueryObjectui64v(timerQuery, GL_QUERY_RESULT, &elapsedNanoSeconds);
      gpuSum += elapsedNanoSeconds / 1'000'000.0;
      sampleCount++;

      renderObject(&outerSphereShader, &outerGlass);

      hdrBuffer->unbindFBO();

      hdrBuffer->bindCBO();
      renderObject(&hdrShader, &screenQuad);

      drawImgui();
      glfwSwapBuffers(window);
      glfwPollEvents();
    }

    delete hdrBuffer;
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
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
  bool isPressed = glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS;
  if (isPressed && !wasPressed) {
    isLocked = !isLocked;
    glfwSetInputMode(window, GLFW_CURSOR,
                     isLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    if (isLocked)
      firstMouseMovement = true;
  }
  wasPressed = isPressed;
}

void framebuffer_size_callback(GLFWwindow *, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
    return;

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
  Sphere *sphere = dynamic_cast<Sphere *>(primitive);
  if (sphere) {
		setupShaderMatrices(shader);
    if (sphere->getName() == "outer") {
      glEnable(GL_BLEND);
      glDepthMask(GL_FALSE);
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
  Quad *quad = dynamic_cast<Quad *>(primitive);
  if(quad) {
  	shader->use();
  	shader->setFloat("exposure", hdrBuffer->getExposure());
  }
  primitive->draw();
}

void renderFilaments(Shader *shader, FilamentSystem *filamentSystem) {
  setupShaderMatrices(shader);
  shader->setFloat("PI", PI);
  shader->setFloat("width", width);
  filamentSystem->drawFilaments();
}

void setupShaderMatrices(Shader *shader) {
  shader->use();
  glm::mat4 model(1.0f);
  shader->setMat4("model", model);
  shader->setMat4("view", camera.getViewMatrix());
  shader->setMat4("projection", camera.getProjectionMatrix());
}

void drawImgui() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  if (ImGui::Begin("Monitor")) {
    if (ImGui::BeginTabBar("Tabs")) {
      if (ImGui::BeginTabItem("Parameters")) {
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Performance")) {
				ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
        ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Text("CPU Geometry Time: %.3f ms", geometryGenerationTime);
        ImGui::Text("CPU Draw Time: %.3f ms", cpuDrawTime);
        ImGui::Text("GPU Draw Time: %.3f ms", gpuDrawTime);
        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }
  }

  ImGui::End();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
