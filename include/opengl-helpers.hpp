#pragma once
#include "config.hpp"
#include "file.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "mouse.hpp"
#include "object.hpp"
#include "shader.hpp"
#include "stb_image.h"
#include "tiny_gltf.h"
#include "vertex-buffers.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>

struct RGBA {
  float r, g, b, a;
};

struct DrawingContext {
  bool useTexture = true;
  RGBA rgba;
  GLenum mode;
  GLsizei indexCount;

  GLint colorLoc;
  GLint useTextureLoc;
  GLint displayModeLoc;
  GLint modelLoc;
  GLint viewLoc;
  GLint projectionLoc;
};

void initialize();
void initializeGLFW();
void setupGlobalWindow();
void initializeImGui();
void setupWindowCallbacks();
void setupGlobalDrawingContext();

void loadMeshToGPU(Mesh& mesh);
void loadSubmeshToGPU(Submesh& submesh);
VertexBuffers getEmptyVertexBuffers();
void loadAttributesIntoVAO(const std::vector<VertexAttribute>& vertexAttributes);
int getStride(const std::vector<VertexAttribute>& vertexAttributes);
void loadVerticesIntoVBO(const std::vector<float>& vertices);
void loadIndicesIntoEBO(const std::vector<unsigned int>& indices);

void loadTextureFromImageToGPU(const std::string& path, GLuint& buffer);
void loadTextureFromGltfToGPU(const tinygltf::Model& model, int textureIndex, GLuint& buffer);
void loadLightsToGPU();

void terminate();
void deleteMaterial(const Material& material);
void deleteObject(const Object& object);
void deleteSubmesh(const Submesh& submesh);
void terminateImGui();
