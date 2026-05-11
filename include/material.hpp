#pragma once
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

struct Material {
  GLuint baseColorTexture = 0;
  GLuint normalTexture = 0;
  GLuint metallicRoughnessTexture = 0;
  GLuint occlusionTexture = 0;
  GLuint emissiveTexture = 0;

  glm::vec4 baseColorFactor = {1.0f, 1.0f, 1.0f, 1.0f};

  float metallicFactor = 0.0f;
  float roughnessFactor = 1.0f;

  glm::vec3 emissiveFactor = {0.0f, 0.0f, 0.0f};

  float occlusionStrength = 1.0f;

  float alphaCutoff = 0.5f;
  float transmission = 0.0f;
};
