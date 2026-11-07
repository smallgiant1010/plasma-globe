#include "shader.hpp"
#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

#define MAX_LOG_SIZE 512

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
  std::ifstream in_str_vertex, in_str_fragment;
  std::string vertexCode, fragmentCode;
  try {
    in_str_vertex.open(vertexPath);
    in_str_fragment.open(fragmentPath);
    std::stringstream vertexStream, fragmentStream;
    vertexStream << in_str_vertex.rdbuf();
    fragmentStream << in_str_fragment.rdbuf();
    in_str_vertex.close();
    in_str_fragment.close();
    vertexCode = vertexStream.str();
    fragmentCode = fragmentStream.str();
  } catch (std::ifstream::failure e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ:\n"
              << e.what() << std::endl;
  }

  const char *vertexSource = vertexCode.c_str();
  const char *fragmentSource = fragmentCode.c_str();

  unsigned int vertexShader, fragmentShader;
  int success{};
  char infoLog[MAX_LOG_SIZE];

  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, MAX_LOG_SIZE, NULL, infoLog);
    std::cout << "ERROR::SHADER::COMPILATION_FAILED:\n" << infoLog << std::endl;
  }

  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, MAX_LOG_SIZE, NULL, infoLog);
    std::cout << "ERROR::SHADER::COMPILATION_FAILED:\n" << infoLog << std::endl;
  }

  shaderID = glCreateProgram();
  glAttachShader(shaderID, vertexShader);
  glAttachShader(shaderID, fragmentShader);
  glLinkProgram(shaderID);
  glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderID, MAX_LOG_SIZE, NULL, infoLog);
    std::cout << "ERROR::PROGRAM::LINKING_FAILED:\n" << infoLog << std::endl;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

Shader::Shader(const Shader &other) { this->shaderID = other.shaderID; }

Shader::~Shader() { glDeleteProgram(shaderID); }

void Shader::use() const { glUseProgram(shaderID); }

void Shader::setInt(const std::string &uniformName, int val) const {
  glUniform1i(glGetUniformLocation(shaderID, uniformName.c_str()), val);
}

void Shader::setBool(const std::string &uniformName, bool val) const {
  glUniform1i(glGetUniformLocation(shaderID, uniformName.c_str()), (int)val);
}

void Shader::setFloat(const std::string &uniformName, float val) const {
  glUniform1f(glGetUniformLocation(shaderID, uniformName.c_str()), val);
}

void Shader::setVec3(const std::string &uniformName, glm::vec3 val) const {
  glUniform3fv(glGetUniformLocation(shaderID, uniformName.c_str()), 1,
               glm::value_ptr(val));
}

void Shader::setMat4(const std::string &uniformName, glm::mat4 val) const {
  glUniformMatrix4fv(glGetUniformLocation(shaderID, uniformName.c_str()), 1,
                     GL_FALSE, glm::value_ptr(val));
}
