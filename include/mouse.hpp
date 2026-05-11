#pragma once
#include "ray.hpp"

#include <GLFW/glfw3.h>

struct Mouse {
  double x;
  double y;
  double prevx;
  double prevy;
  float xoffset = 0.0f;
  float yoffset = 0.0f;
  float sensitivity = 0.1f;
  bool moved = false;

  void moveTo(double xpos, double ypos);
};

void moveMouse(GLFWwindow*, double xpos, double ypos);

void onCursorPos(GLFWwindow* window, double xpos, double ypos);
void onMouseButton(GLFWwindow* window, int button, int action, int mods);
