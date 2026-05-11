#pragma once
#include <GL/gl.h>

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

struct WindowConfig {
  int width;
  int height;
  std::string title;
};

struct ShadersConfig {
  std::string vertex;
  std::string fragment;
};

struct VertexAttribute {
  GLuint index;
  GLint size;
  GLint offset;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(VertexAttribute, index, size, offset)
};

struct TriangleConfig {
  std::vector<VertexAttribute> attributes;
  ShadersConfig shaders;
};

struct Config {
  WindowConfig window;
  std::string mesh;
  TriangleConfig triangle;
};

void loadGlobalConfig(std::string path = "config.json");
