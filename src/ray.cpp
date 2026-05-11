#include "ray.hpp"

#include "globals.hpp"

Ray screenToRay(double mouseX, double mouseY) {
  Config& config = Globals::config;
  Camera& camera = Globals::camera;

  float x = (2.0f * mouseX) / config.window.width - 1.0f;
  float y = 1.0f - (2.0f * mouseY) / config.window.height;

  glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
  glm::vec4 rayEye = glm::inverse(camera.projection) * rayClip;
  rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
  glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(camera.view) * rayEye));

  return {camera.pos, rayWorld};
}

bool intersectRayWithAABB(const Ray& ray, const AABB& aabb) {
  glm::vec3 invDir = 1.0f / ray.direction;

  glm::vec3 t1 = (aabb.min - ray.origin) * invDir;
  glm::vec3 t2 = (aabb.max - ray.origin) * invDir;

  glm::vec3 tMin = glm::min(t1, t2);
  glm::vec3 tMax = glm::max(t1, t2);

  float tNear = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
  float tFar = glm::min(glm::min(tMax.x, tMax.y), tMax.z);

  return tNear <= tFar && tFar >= 0.0f;
}
