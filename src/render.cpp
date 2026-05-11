#include "render.hpp"

#include "globals.hpp"

void render() {
  Globals::time.refresh();

  clearWindow();

  if (Globals::viewMode == ViewMode::Absolute)
    processKeyPress();

  glUseProgram(Globals::program);

  Camera& camera = Globals::camera;
  camera.refreshVP();
  camera.uploadVPtoGPU(Globals::program);

  glUniform3f(
    glGetUniformLocation(Globals::program, "viewPos"),
    camera.pos.x,
    camera.pos.y,
    camera.pos.z
  );

  DrawingContext& ctx = Globals::drawingContext;

  if (Globals::displayMode != DisplayMode::Wireframe) {
    ctx.mode = GL_FILL;
    ctx.rgba = {0.5f, 0.5f, 0.5f, 1.0f};
  } else {
    ctx.mode = GL_LINE;
    ctx.rgba = {0.1f, 0.1f, 0.1f, 1.0f};
  }

  drawScene(Globals::scene);

  renderUI();

  glfwSwapBuffers(Globals::window);
  glfwPollEvents();
}

void drawScene(const Scene& scene) {
  loadLightsToGPU();
  for (auto& object : scene.objects) drawObject(object);
}

void drawObject(const Object& object, glm::mat4 parentMatrix) {
  DrawingContext& ctx = Globals::drawingContext;
  glm::mat4 worldMatrix = parentMatrix * object.transform.matrix;
  glUniformMatrix4fv(ctx.modelLoc, 1, GL_FALSE, glm::value_ptr(worldMatrix));
  drawMesh(object.mesh);

  if (object.selected)
    drawAABB(object.aabb);

  for (auto& child : object.children) drawObject(child, worldMatrix);
}

void drawMesh(const Mesh& mesh) {
  glPolygonMode(GL_FRONT_AND_BACK, Globals::drawingContext.mode);
  for (auto& submesh : mesh.submeshes) drawSubmesh(submesh);
}

void drawAABB(const AABB& aabb) {
  DrawingContext& ctx = Globals::drawingContext;
  auto prevMode = ctx.mode;
  auto prevColor = ctx.rgba;

  ctx.mode = GL_LINE;
  ctx.rgba = {1.0f, 0.5f, 0.0f, 1.0f};

  Submesh box = makeBoxSubmesh(aabb.min, aabb.max, 0.001);
  loadSubmeshToGPU(box);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glUniform4f(ctx.colorLoc, ctx.rgba.r, ctx.rgba.g, ctx.rgba.b, 1.0f);
  glUniform1f(ctx.useTextureLoc, false);
  glBindVertexArray(box.buffers.VAO);
  glDrawElements(GL_LINES, box.indices.size(), GL_UNSIGNED_INT, 0);

  deleteSubmesh(box);

  ctx.mode = prevMode;
  ctx.rgba = prevColor;
}

Submesh makeBoxSubmesh(glm::vec3 min, glm::vec3 max, float eps) {
  min -= glm::vec3(eps);
  max += glm::vec3(eps);

  Submesh submesh;
  submesh.materialIndex = -1;
  submesh.vertices = {
    min.x, min.y, min.z, 0, 0, 1, 0, 0, max.x, min.y, min.z, 0, 0, 1, 1, 0,
    max.x, max.y, min.z, 0, 0, 1, 1, 1, min.x, max.y, min.z, 0, 0, 1, 0, 1,
    min.x, min.y, max.z, 0, 0, 1, 0, 0, max.x, min.y, max.z, 0, 0, 1, 1, 0,
    max.x, max.y, max.z, 0, 0, 1, 1, 1, min.x, max.y, max.z, 0, 0, 1, 0, 1,
  };
  submesh.indices = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};
  return submesh;
}

void drawSubmesh(const Submesh& submesh) {
  DrawingContext& ctx = Globals::drawingContext;

  bool materialExists =
    submesh.materialIndex > -1 && submesh.materialIndex < (int)Globals::materials.size();

  glUniform1i(ctx.displayModeLoc, static_cast<int>(Globals::displayMode));
  glUniform1f(ctx.useTextureLoc, ctx.useTexture);

  if (Globals::displayMode == DisplayMode::Wireframe || !materialExists)
    drawSubmeshWithContext(submesh);
  else
    drawSubmeshWithMaterial(submesh);
}

void drawSubmeshWithContext(const Submesh& submesh) {
  DrawingContext& ctx = Globals::drawingContext;
  glUniform4f(ctx.colorLoc, ctx.rgba.r, ctx.rgba.g, ctx.rgba.b, 1.0f);
  glUniform1f(ctx.useTextureLoc, false);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(submesh.buffers.VAO);
  glDrawElements(GL_TRIANGLES, submesh.indices.size(), GL_UNSIGNED_INT, 0);
}

void drawSubmeshWithMaterial(const Submesh& submesh) {
  DrawingContext& ctx = Globals::drawingContext;
  Material& material = Globals::materials[submesh.materialIndex];

  if (material.baseColorTexture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material.baseColorTexture);
  } else {
    glUniform4f(
      ctx.colorLoc,
      material.baseColorFactor[0],
      material.baseColorFactor[1],
      material.baseColorFactor[2],
      material.baseColorFactor[3]
    );
  }

  glBindVertexArray(submesh.buffers.VAO);
  glDrawElements(GL_TRIANGLES, submesh.indices.size(), GL_UNSIGNED_INT, 0);
}

void clearWindow(GLclampf r, GLclampf g, GLclampf b, GLclampf a) {
  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
