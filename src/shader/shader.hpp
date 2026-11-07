#pragma once
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
  unsigned int shaderID;
  Shader(const std::string &vertexPath, const std::string &fragmentPath);
  ~Shader();
  Shader(const Shader &other);
  void use() const;
  void setInt(const std::string &uniformName, int val) const;
  void setBool(const std::string &uniformName, bool val) const;
  void setFloat(const std::string &uniformName, float val) const;
  void setVec3(const std::string &uniformName, glm::vec3 val) const;
  void setMat4(const std::string &uniformName, glm::mat4 val) const;
};
