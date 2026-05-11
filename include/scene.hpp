#pragma once
#include "light.hpp"
#include "object.hpp"
#include "vector"

struct Scene {
  std::vector<Object> objects;
  std::vector<Light> lights = {Light()};
};

const int MAX_LIGHTS_NUM = 32;

Scene mergeScenes(const Scene& a, const Scene& b);
