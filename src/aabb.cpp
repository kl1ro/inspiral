#include "aabb.hpp"

AABB computeAABB(const Mesh& mesh) {
  glm::vec3 min = glm::vec3(FLT_MAX);
  glm::vec3 max = glm::vec3(-FLT_MAX);

  for (auto& submesh : mesh.submeshes) {
    for (int i = 0; i < submesh.vertices.size(); i += 8) {
      glm::vec3 pos =
        glm::vec3(submesh.vertices[i], submesh.vertices[i + 1], submesh.vertices[i + 2]);

      min = glm::min(min, pos);
      max = glm::max(max, pos);
    }
  }

  return {min, max};
}

AABB transformAABBtoWorldSpace(const AABB& aabb, const glm::mat4& matrix) {
  glm::vec3 corners[8] = {
    {aabb.min.x, aabb.min.y, aabb.min.z},
    {aabb.max.x, aabb.min.y, aabb.min.z},
    {aabb.min.x, aabb.max.y, aabb.min.z},
    {aabb.max.x, aabb.max.y, aabb.min.z},
    {aabb.min.x, aabb.min.y, aabb.max.z},
    {aabb.max.x, aabb.min.y, aabb.max.z},
    {aabb.min.x, aabb.max.y, aabb.max.z},
    {aabb.max.x, aabb.max.y, aabb.max.z},
  };

  glm::vec3 min = glm::vec3(FLT_MAX);
  glm::vec3 max = glm::vec3(-FLT_MAX);

  for (auto& corner : corners) {
    glm::vec3 transformed = glm::vec3(matrix * glm::vec4(corner, 1.0f));
    min = glm::min(min, transformed);
    max = glm::max(max, transformed);
  }

  return {min, max};
}
