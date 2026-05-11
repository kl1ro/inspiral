#pragma once
#include "keyboard.hpp"
#include "object.hpp"
#include "opengl-helpers.hpp"
#include "ui.hpp"

void render();
void drawScene(const Scene& scene);
void drawObject(const Object& object, glm::mat4 parentMatrix = glm::mat4(1.0f));
void drawAABB(const AABB& aabb);
Submesh makeBoxSubmesh(glm::vec3 min, glm::vec3 max, float eps = 0.0f);
void drawMesh(const Mesh& mesh);
void drawSubmesh(const Submesh& submesh);
void drawSubmeshWithContext(const Submesh& submesh);
void drawSubmeshWithMaterial(const Submesh& submesh);

void clearWindow(
  GLclampf red = 0.247f,
  GLclampf green = 0.247f,
  GLclampf blue = 0.247f,
  GLclampf alpha = 1.0f
);
