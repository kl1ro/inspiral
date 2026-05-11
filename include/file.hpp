#pragma once
#include "material.hpp"
#include "mesh.hpp"
#include "opengl-helpers.hpp"
#include "scene.hpp"
#include "tiny_gltf.h"
#include "tiny_obj_loader.h"

#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

struct TinyObj {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> tinyMaterials;
  std::string warn, err;
  std::string baseDir;
  bool success;
};

struct TinyGltf {
  tinygltf::Model model;
  tinygltf::TinyGLTF loader;
  std::string err, warn;
  bool success;
};

std::string readFile(const std::string& path);
std::string openFileDialog(const std::string& filter = "*");

void importObj(const std::string& path);

static TinyObj loadTinyObj(const std::string& path);
static Object tinyObjToObject(const TinyObj& tinyObj);
static Mesh tinyObjToMesh(const TinyObj& tinyObj);
static std::vector<Submesh>
shapeToSubmeshes(const tinyobj::shape_t& shape, const tinyobj::attrib_t& attrib);
static std::vector<Material> tinyObjToMaterials(const TinyObj& tinyObj);

void importGltf(const std::string& path);
void openGltf(const std::string& path);

static TinyGltf loadTinyGltf(const std::string& path);
static Scene tinyGltfToScene(const TinyGltf& tinyGltf);
static std::vector<Object> tinyGltfToObjects(const TinyGltf& tinyGltf);
static std::vector<Light> tinyGltfToLights(const TinyGltf& tinyGltf);
static Object tinyGltfToObject(const TinyGltf& tinyGltf, int nodeIndex);
static Mesh tinyGltfToMesh(const TinyGltf& tinyGltf, int meshIndex);
static Transform nodeToTransform(const tinygltf::Node& node);
static std::vector<Material> tinyGltfToMaterials(const TinyGltf& tinyGltf);

static const float* getBufferData(const tinygltf::Model& model, int accessorIndex);
static const unsigned short* getIndexData(const tinygltf::Model& model, int accessorIndex);
