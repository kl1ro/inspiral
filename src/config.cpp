#include "config.hpp"

#include "globals.hpp"

void loadGlobalConfig(std::string path) {
  Config& config = Globals::config;

  if (config.window.title.length())
    return;

  std::ifstream file(path);
  nlohmann::json j;
  file >> j;

  config.window.width = j["window"].value("width", 800);
  config.window.height = j["window"].value("height", 600);
  config.window.title = j["window"].value("title", "OpenGL Triangle");

  config.triangle.attributes = j["triangle"]["attributes"].get<std::vector<VertexAttribute>>();
  config.triangle.shaders.vertex = j["triangle"]["shaders"].value("vertex", "shaders/vertex.glsl");
  config.triangle.shaders.fragment =
    j["triangle"]["shaders"].value("fragment", "shaders/fragment.glsl");

  config.mesh = j.value("mesh", "cube.obj");
}
