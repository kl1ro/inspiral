#include "scene.hpp"

Scene mergeScenes(const Scene& a, const Scene& b) {
  Scene scene;

  scene.lights.insert(scene.lights.end(), a.lights.begin(), a.lights.end());
  scene.lights.insert(scene.lights.end(), b.lights.begin(), b.lights.end());

  scene.objects.insert(scene.objects.end(), a.objects.begin(), a.objects.end());
  scene.objects.insert(scene.objects.end(), b.objects.begin(), b.objects.end());

  return scene;
}
