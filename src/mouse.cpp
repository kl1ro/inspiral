#include "mouse.hpp"

#include "globals.hpp"

void Mouse::moveTo(double xpos, double ypos) {
  if (!moved) {
    prevx = xpos;
    prevy = ypos;
    moved = true;
    return;
  }

  xoffset = (xpos - prevx) * sensitivity;
  yoffset = (prevy - ypos) * sensitivity;
  prevx = xpos;
  prevy = ypos;
}

void moveMouse(GLFWwindow*, double xpos, double ypos) {
  Mouse& mouse = Globals::mouse;
  Camera& camera = Globals::camera;

  mouse.moveTo(xpos, ypos);
  camera.rotate(mouse.xoffset, mouse.yoffset);
}

void onCursorPos(GLFWwindow* window, double xpos, double ypos) {
  ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

  if (Globals::viewMode == ViewMode::Absolute)
    moveMouse(window, xpos, ypos);
}

void onMouseButton(GLFWwindow* window, int button, int action, int mods) {
  ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

  if (ImGui::GetIO().WantCaptureMouse)
    return;

  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    Ray ray = screenToRay(mouseX, mouseY);

    for (auto& obj : Globals::scene.objects) {
      AABB wAABB = transformAABBtoWorldSpace(obj.aabb, obj.transform.matrix);
      obj.selected = intersectRayWithAABB(ray, wAABB);
    }
  }
}
