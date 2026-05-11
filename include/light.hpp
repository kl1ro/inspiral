#pragma once
#include "transform.hpp"

#include <iostream>

struct Light {
  std::string name;
  glm::vec3 position = glm::vec3(2.0f, 1.0f, 3.0f);
  glm::vec3 color = glm::vec3(1.0f);
  float intensity = 1.0f;
  bool visible = true;
};
