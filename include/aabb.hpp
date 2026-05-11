#pragma once
#include "mesh.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct AABB {
  glm::vec3 min;
  glm::vec3 max;
};

AABB computeAABB(const Mesh& mesh);
AABB transformAABBtoWorldSpace(const AABB& aabb, const glm::mat4& matrix);
