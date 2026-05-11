#include "camera.hpp"

#include "globals.hpp"

void Camera::rotate(float xoffset, float yoffset) {
  yaw += xoffset;
  pitch += yoffset;
  pitch = glm::clamp(pitch, -89.0f, 89.0f);

  glm::vec3 dir;
  dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  dir.y = sin(glm::radians(pitch));
  dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  front = glm::normalize(dir);
}

void Camera::refreshVP() {
  Config& config = Globals::config;

  view = glm::lookAt(this->pos, this->pos + this->front, this->up);

  projection = glm::perspective(
    glm::radians(45.0f),
    float(config.window.width) / float(config.window.height),
    0.1f,
    1000.0f
  );
}

void Camera::uploadVPtoGPU(GLuint program) {
  glUniformMatrix4fv(Globals::drawingContext.viewLoc, 1, GL_FALSE, glm::value_ptr(view));

  glUniformMatrix4fv(
    Globals::drawingContext.projectionLoc,
    1,
    GL_FALSE,
    glm::value_ptr(projection)
  );
}

void toggleViewMode() {
  if (Globals::viewMode == ViewMode::Absolute)
    setViewModeToRelative();
  else
    setViewModeToAbsolute();
}

void setViewModeToAbsolute() {
  Globals::viewMode = ViewMode::Absolute;
  glfwSetCursorPosCallback(Globals::window, moveMouse);
  glfwSetInputMode(Globals::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

void setViewModeToRelative() {
  Globals::viewMode = ViewMode::Relative;
  glfwSetCursorPosCallback(Globals::window, onCursorPos);
  glfwSetInputMode(Globals::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  Globals::mouse.moved = false;
  ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

void advanceDisplayMode() {
  DisplayMode& mode = Globals::displayMode;

  switch (mode) {
  case DisplayMode::Wireframe:
    mode = DisplayMode::Textured;
    break;
  case DisplayMode::Textured:
    mode = DisplayMode::Shaded;
    break;
  case DisplayMode::Shaded:
    mode = DisplayMode::Wireframe;
    break;
  }
}
