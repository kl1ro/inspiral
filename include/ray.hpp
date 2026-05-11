#pragma once
#include "aabb.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;
};

Ray screenToRay(double screenX, double screenY);
bool intersectRayWithAABB(const Ray& ray, const AABB& aabb);
