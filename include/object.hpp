#pragma once
#include "aabb.hpp"
#include "mesh.hpp"
#include "transform.hpp"

#include <iostream>

struct Object {
  std::string name;
  Mesh mesh;
  Transform transform;
  AABB aabb;
  std::vector<Object> children;
  bool selected = false;
};
