#include "opengl-helpers.hpp"

#include "globals.hpp"
#include "keyboard.hpp"

void initialize() {
  initializeGLFW();
  loadGlobalConfig();

  setupGlobalWindow();
  initializeImGui();
  setupWindowCallbacks();

  setupGlobalProgram();
  setupGlobalDrawingContext();
}

void initializeGLFW() {
  if (!glfwInit()) {
    std::cerr << "Failed to init GLFW\n";
    std::exit(1);
  }
}

void initializeImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(Globals::window, true);
  ImGui_ImplOpenGL3_Init("#version 330");
}

void setupWindowCallbacks() {
  GLFWwindow*& window = Globals::window;
  glfwSetKeyCallback(window, onKeyPress);
  glfwSetMouseButtonCallback(window, onMouseButton);
}

void setupGlobalWindow() {
  GLFWwindow*& window = Globals::window;
  WindowConfig& config = Globals::config.window;

  if (window)
    return;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_STENCIL_BITS, 8);

  window = glfwCreateWindow(config.width, config.height, config.title.c_str(), nullptr, nullptr);

  if (!window) {
    std::cerr << "Failed to create window\n";
    glfwTerminate();
    std::exit(1);
  }

  glfwMakeContextCurrent(window);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glfwSwapInterval(1);

  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Renderer:       " << glGetString(GL_RENDERER) << std::endl;
}

void setupGlobalDrawingContext() {
  DrawingContext& ctx = Globals::drawingContext;
  GLuint& program = Globals::program;

  ctx.colorLoc = glGetUniformLocation(program, "color");
  ctx.displayModeLoc = glGetUniformLocation(program, "displayMode");
  ctx.modelLoc = glGetUniformLocation(program, "model");
  ctx.viewLoc = glGetUniformLocation(program, "view");
  ctx.projectionLoc = glGetUniformLocation(program, "projection");
  ctx.useTextureLoc = glGetUniformLocation(program, "useTexture");
}

VertexBuffers getEmptyVertexBuffers() {
  VertexBuffers buffers;

  glGenVertexArrays(1, &buffers.VAO);
  glGenBuffers(1, &buffers.VBO);
  glGenBuffers(1, &buffers.EBO);
  glBindVertexArray(buffers.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.EBO);

  return buffers;
}

void loadMeshToGPU(Mesh& mesh) {
  for (auto& submesh : mesh.submeshes) loadSubmeshToGPU(submesh);
}

void loadSubmeshToGPU(Submesh& submesh) {
  submesh.buffers = getEmptyVertexBuffers();
  loadAttributesIntoVAO(Globals::config.triangle.attributes);
  loadVerticesIntoVBO(submesh.vertices);
  loadIndicesIntoEBO(submesh.indices);
}

void loadAttributesIntoVAO(const std::vector<VertexAttribute>& vertexAttributes) {
  auto stride = getStride(vertexAttributes);

  for (auto& attribute : vertexAttributes) {
    glVertexAttribPointer(
      attribute.index,
      attribute.size,
      GL_FLOAT,
      GL_FALSE,
      stride * sizeof(float),
      (void*)(attribute.offset * sizeof(float))
    );

    glEnableVertexAttribArray(attribute.index);
  }
}

int getStride(const std::vector<VertexAttribute>& vertexAttributes) {
  int stride = 0;
  for (auto& attribute : vertexAttributes) stride += attribute.size;
  return stride;
}

void loadVerticesIntoVBO(const std::vector<float>& vertices) {
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
}

void loadIndicesIntoEBO(const std::vector<unsigned int>& indices) {
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    indices.size() * sizeof(unsigned int),
    indices.data(),
    GL_STATIC_DRAW
  );
}

void loadTextureFromImageToGPU(const std::string& path, GLuint& buffer) {
  int width, height, channels;

  stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

  if (!data) {
    std::cerr << "Failed to load texture: " << path << "\n";
    std::exit(1);
  }

  glGenTextures(1, &buffer);
  glBindTexture(GL_TEXTURE_2D, buffer);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);
}

void loadTextureFromGltfToGPU(const tinygltf::Model& model, int textureIndex, GLuint& buffer) {
  if (textureIndex < 0)
    return;

  const tinygltf::Texture& tex = model.textures[textureIndex];
  const tinygltf::Image& image = model.images[tex.source];

  glGenTextures(1, &buffer);
  glBindTexture(GL_TEXTURE_2D, buffer);

  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGBA,
    image.width,
    image.height,
    0,
    image.component == 3 ? GL_RGB : GL_RGBA,
    GL_UNSIGNED_BYTE,
    image.image.data()
  );

  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void loadLightsToGPU() {
  GLuint& program = Globals::program;

  glUniform1i(glGetUniformLocation(program, "numLights"), Globals::scene.lights.size());

  for (int i = 0; i < Globals::scene.lights.size(); i++) {
    std::string base = "lights[" + std::to_string(i) + "]";
    Light& light = Globals::scene.lights[i];

    glUniform3fv(
      glGetUniformLocation(program, (base + ".position").c_str()),
      1,
      glm::value_ptr(light.position)
    );

    glUniform3fv(
      glGetUniformLocation(program, (base + ".color").c_str()),
      1,
      glm::value_ptr(light.color)
    );

    glUniform1f(glGetUniformLocation(program, (base + ".intensity").c_str()), light.intensity);
  }
}

void terminate() {
  glDeleteProgram(Globals::program);
  for (auto& material : Globals::materials) deleteMaterial(material);
  for (auto& object : Globals::scene.objects) deleteObject(object);
  terminateImGui();
}

void deleteMaterial(const Material& material) {
  glDeleteTextures(1, &material.baseColorTexture);
}

void deleteObject(const Object& object) {
  for (auto& submesh : object.mesh.submeshes) deleteSubmesh(submesh);
}

void deleteSubmesh(const Submesh& submesh) {
  glDeleteVertexArrays(1, &submesh.buffers.VAO);
  glDeleteBuffers(1, &submesh.buffers.VBO);
  glDeleteBuffers(1, &submesh.buffers.EBO);
}

void terminateImGui() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();
}
