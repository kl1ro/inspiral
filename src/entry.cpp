#include "render.hpp"

int main() {
  initialize();
  importObj(Globals::config.mesh);
  while (!glfwWindowShouldClose(Globals::window)) render();
  terminate();
}
