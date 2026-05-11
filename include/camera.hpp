#pragma once
#include "opengl-helpers.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Camera {
  glm::vec3 pos = glm::vec3(7.36f, 4.96f, 6.93f);
  glm::vec3 front = glm::normalize(glm::vec3(0.0f) - pos);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::mat4 view;
  glm::mat4 projection;
  float yaw = glm::degrees(atan2(front.z, front.x));
  float pitch = glm::degrees(asin(front.y));
  float speed = 2.5f;

  void rotate(float xoffset, float yoffset);
  void refreshVP();
  void uploadVPtoGPU(GLuint program);
};

enum class ViewMode { Absolute, Relative };
enum class DisplayMode { Wireframe, Textured, Shaded };

void toggleViewMode();
void setViewModeToAbsolute();
void setViewModeToRelative();

void advanceDisplayMode();
