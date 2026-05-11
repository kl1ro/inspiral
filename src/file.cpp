#include "file.hpp"

#include "globals.hpp"

std::string readFile(const std::string& path) {
  std::ifstream file(path);

  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << path << "\n";
    std::exit(1);
  }

  std::stringstream buf;
  buf << file.rdbuf();
  return buf.str();
}

std::string openFileDialog(const std::string& filter) {
  std::string command = "zenity --file-selection --file-filter='" + filter + " | " + filter + "'";
  FILE* f = popen(command.c_str(), "r");

  if (!f)
    return "";

  char path[1024] = {};
  fgets(path, sizeof(path), f);
  pclose(f);
  std::string result(path);

  if (!result.empty() && result.back() == '\n')
    result.pop_back();

  return result;
}

void importObj(const std::string& path) {
  auto tinyObj = loadTinyObj(path);

  auto object = tinyObjToObject(tinyObj);
  loadMeshToGPU(object.mesh);
  auto materials = tinyObjToMaterials(tinyObj);

  Globals::materials.insert(Globals::materials.end(), materials.begin(), materials.end());
  Globals::scene.objects.push_back(object);
}

static TinyObj loadTinyObj(const std::string& path) {
  TinyObj tinyObj;

  tinyObj.baseDir = std::filesystem::path(path).parent_path().string();

  tinyObj.success = tinyobj::LoadObj(
    &tinyObj.attrib,
    &tinyObj.shapes,
    &tinyObj.tinyMaterials,
    &tinyObj.warn,
    &tinyObj.err,
    path.c_str(),
    tinyObj.baseDir.c_str(),
    true
  );

  if (!tinyObj.err.empty())
    std::cerr << "OBJ error: " << tinyObj.err << "\n";

  if (!tinyObj.success)
    throw std::runtime_error("Failed to load OBJ: " + path);

  return tinyObj;
}

static std::vector<Material> tinyObjToMaterials(const TinyObj& tinyObj) {
  std::vector<Material> materials;

  for (auto& mat : tinyObj.tinyMaterials) {
    Material material = {};

    if (!mat.diffuse_texname.empty())
      loadTextureFromImageToGPU(
        tinyObj.baseDir + "/" + mat.diffuse_texname,
        material.baseColorTexture
      );

    material.baseColorFactor =
      glm::vec4(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], mat.dissolve);

    materials.push_back(material);
  }

  return materials;
}

static Object tinyObjToObject(const TinyObj& tinyObj) {
  Object object;
  object.mesh = tinyObjToMesh(tinyObj);
  object.aabb = computeAABB(object.mesh);
  return object;
}

static Mesh tinyObjToMesh(const TinyObj& tinyObj) {
  Mesh mesh;

  for (auto& shape : tinyObj.shapes) {
    auto submeshes = shapeToSubmeshes(shape, tinyObj.attrib);
    mesh.submeshes.insert(mesh.submeshes.end(), submeshes.begin(), submeshes.end());
  }

  return mesh;
}

static std::vector<Submesh>
shapeToSubmeshes(const tinyobj::shape_t& shape, const tinyobj::attrib_t& attrib) {
  std::unordered_map<int, Submesh> submeshMap;

  for (int f = 0; f < (int)shape.mesh.material_ids.size(); f++) {
    int matId = shape.mesh.material_ids[f];
    Submesh& submesh = submeshMap[matId];

    submesh.materialIndex = Globals::materials.size() + matId;

    for (int v = 0; v < 3; v++) {
      tinyobj::index_t idx = shape.mesh.indices[f * 3 + v];

      float vx = attrib.vertices[3 * idx.vertex_index + 0];
      float vy = attrib.vertices[3 * idx.vertex_index + 1];
      float vz = attrib.vertices[3 * idx.vertex_index + 2];
      float u = idx.texcoord_index >= 0 ? attrib.texcoords[2 * idx.texcoord_index + 0] : 0.0f;
      float v_ = idx.texcoord_index >= 0 ? attrib.texcoords[2 * idx.texcoord_index + 1] : 0.0f;

      float nx = idx.normal_index >= 0 ? attrib.normals[3 * idx.normal_index + 0] : 0.0f;
      float ny = idx.normal_index >= 0 ? attrib.normals[3 * idx.normal_index + 1] : 0.0f;
      float nz = idx.normal_index >= 0 ? attrib.normals[3 * idx.normal_index + 2] : 1.0f;

      submesh.vertices.insert(submesh.vertices.end(), {vx, vy, vz, nx, ny, nz, u, v_});
      submesh.indices.push_back(submesh.indices.size());
    }
  }

  std::vector<Submesh> submeshes(submeshMap.size());

  std::transform(submeshMap.begin(), submeshMap.end(), submeshes.begin(), [](auto& pair) {
    return pair.second;
  });

  return submeshes;
}

void importGltf(const std::string& path) {
  auto tinyGltf = loadTinyGltf(path);

  auto scene = tinyGltfToScene(tinyGltf);
  auto materials = tinyGltfToMaterials(tinyGltf);

  Globals::materials.insert(Globals::materials.end(), materials.begin(), materials.end());
  Globals::scene = mergeScenes(Globals::scene, scene);
}

void openGltf(const std::string& path) {
  auto tinyGltf = loadTinyGltf(path);

  auto scene = tinyGltfToScene(tinyGltf);
  auto materials = tinyGltfToMaterials(tinyGltf);

  Globals::materials.insert(Globals::materials.end(), materials.begin(), materials.end());
  Globals::scene = scene;
}

static TinyGltf loadTinyGltf(const std::string& path) {
  TinyGltf tinyGltf;

  bool success =
    path.ends_with(".glb")
      ? tinyGltf.loader.LoadBinaryFromFile(&tinyGltf.model, &tinyGltf.err, &tinyGltf.warn, path)
      : tinyGltf.loader.LoadASCIIFromFile(&tinyGltf.model, &tinyGltf.err, &tinyGltf.warn, path);

  if (!tinyGltf.warn.empty())
    std::cerr << "GLTF warning: " << tinyGltf.warn << "\n";

  if (!tinyGltf.err.empty())
    std::cerr << "GLTF error: " << tinyGltf.err << "\n";

  if (!success)
    throw std::runtime_error("Failed to load GLTF: " + path);

  return tinyGltf;
}

static Scene tinyGltfToScene(const TinyGltf& tinyGltf) {
  auto& model = tinyGltf.model;

  Scene scene;
  scene.objects = tinyGltfToObjects(tinyGltf);
  auto lights = tinyGltfToLights(tinyGltf);
  scene.lights.insert(scene.lights.end(), lights.begin(), lights.end());

  return scene;
}

static std::vector<Object> tinyGltfToObjects(const TinyGltf& tinyGltf) {
  auto& model = tinyGltf.model;
  auto& gltfScene = model.scenes[model.defaultScene >= 0 ? model.defaultScene : 0];

  std::vector<Object> objects;

  for (int nodeIndex : gltfScene.nodes) {
    auto object = tinyGltfToObject(tinyGltf, nodeIndex);
    objects.push_back(object);
  }

  return objects;
}

static std::vector<Light> tinyGltfToLights(const TinyGltf& tinyGltf) {
  auto& model = tinyGltf.model;
  auto& gltfScene = model.scenes[model.defaultScene >= 0 ? model.defaultScene : 0];

  std::vector<Light> lights;

  if (!model.extensions.count("KHR_lights_punctual"))
    return lights;

  auto& lightsJson = model.extensions.at("KHR_lights_punctual").Get("lights");

  for (int nodeIndex : gltfScene.nodes) {
    const tinygltf::Node& node = model.nodes[nodeIndex];

    if (!node.extensions.count("KHR_lights_punctual"))
      continue;

    int lightIndex = node.extensions.at("KHR_lights_punctual").Get("light").GetNumberAsInt();
    auto& lightJson = lightsJson.Get(lightIndex);

    Light light;
    light.name = node.name;

    if (node.translation.size() == 3)
      light.position = glm::vec3(node.translation[0], node.translation[1], node.translation[2]);

    if (lightJson.Has("color")) {
      auto& c = lightJson.Get("color");
      light.color = glm::vec3(
        c.Get(0).GetNumberAsDouble(),
        c.Get(1).GetNumberAsDouble(),
        c.Get(2).GetNumberAsDouble()
      );
    }

    if (lightJson.Has("intensity"))
      light.intensity = lightJson.Get("intensity").GetNumberAsDouble();

    lights.push_back(light);
  }

  return lights;
}

static std::vector<Material> tinyGltfToMaterials(const TinyGltf& tinyGltf) {
  std::vector<Material> materials;
  auto& model = tinyGltf.model;

  for (auto& mat : tinyGltf.model.materials) {
    Material material = {};
    auto& pbr = mat.pbrMetallicRoughness;

    material.baseColorFactor = glm::vec4(
      pbr.baseColorFactor[0],
      pbr.baseColorFactor[1],
      pbr.baseColorFactor[2],
      pbr.baseColorFactor[3]
    );

    loadTextureFromGltfToGPU(model, pbr.baseColorTexture.index, material.baseColorTexture);
    loadTextureFromGltfToGPU(model, mat.normalTexture.index, material.normalTexture);
    loadTextureFromGltfToGPU(model, mat.emissiveTexture.index, material.emissiveTexture);

    material.emissiveFactor =
      glm::vec3(mat.emissiveFactor[0], mat.emissiveFactor[1], mat.emissiveFactor[2]);

    materials.push_back(material);
  }

  return materials;
}

static Object tinyGltfToObject(const TinyGltf& tinyGltf, int nodeIndex) {
  auto& model = tinyGltf.model;
  const tinygltf::Node& node = model.nodes[nodeIndex];

  Object object;
  object.name = node.name;
  object.transform = nodeToTransform(node);

  if (node.mesh >= 0)
    object.mesh = tinyGltfToMesh(tinyGltf, node.mesh);

  for (int childIndex : node.children) {
    auto child = tinyGltfToObject(tinyGltf, childIndex);
    object.children.push_back(child);
  }

  object.aabb = computeAABB(object.mesh);
  loadMeshToGPU(object.mesh);

  return object;
}

static Transform nodeToTransform(const tinygltf::Node& node) {
  Transform transform;

  if (node.translation.size() == 3)
    transform.setPosition(glm::vec3(node.translation[0], node.translation[1], node.translation[2]));

  if (node.scale.size() == 3)
    transform.setScale(glm::vec3(node.scale[0], node.scale[1], node.scale[2]));

  if (node.rotation.size() == 4) {
    glm::quat q(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
    glm::vec3 euler = glm::degrees(glm::eulerAngles(q));
    transform.setRotation(euler);
  }

  return transform;
}

static Mesh tinyGltfToMesh(const TinyGltf& tinyGltf, int meshIndex) {
  auto& model = tinyGltf.model;
  const auto& gltfMesh = model.meshes[meshIndex];

  Mesh mesh;

  for (auto& primitive : gltfMesh.primitives) {
    Submesh submesh;
    submesh.materialIndex =
      Globals::materials.size() + (primitive.material >= 0 ? primitive.material : 0);

    const float* positions = nullptr;
    const float* normals = nullptr;
    const float* uvs = nullptr;
    int vertexCount = 0;

    if (primitive.attributes.count("POSITION")) {
      positions = getBufferData(model, primitive.attributes.at("POSITION"));
      vertexCount = model.accessors[primitive.attributes.at("POSITION")].count;
    }

    if (primitive.attributes.count("NORMAL"))
      normals = getBufferData(model, primitive.attributes.at("NORMAL"));

    if (primitive.attributes.count("TEXCOORD_0"))
      uvs = getBufferData(model, primitive.attributes.at("TEXCOORD_0"));

    for (int i = 0; i < vertexCount; i++) {
      submesh.vertices.push_back(positions ? positions[i * 3 + 0] : 0.0f);
      submesh.vertices.push_back(positions ? positions[i * 3 + 1] : 0.0f);
      submesh.vertices.push_back(positions ? positions[i * 3 + 2] : 0.0f);
      submesh.vertices.push_back(normals ? normals[i * 3 + 0] : 0.0f);
      submesh.vertices.push_back(normals ? normals[i * 3 + 1] : 0.0f);
      submesh.vertices.push_back(normals ? normals[i * 3 + 2] : 1.0f);
      submesh.vertices.push_back(uvs ? uvs[i * 2 + 0] : 0.0f);
      submesh.vertices.push_back(uvs ? uvs[i * 2 + 1] : 0.0f);
    }

    if (primitive.indices >= 0) {
      const tinygltf::Accessor& idxAccessor = model.accessors[primitive.indices];
      int indexCount = idxAccessor.count;
      if (idxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
        const unsigned short* idx = getIndexData(model, primitive.indices);
        for (int i = 0; i < indexCount; i++) submesh.indices.push_back(idx[i]);
      } else if (idxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
        const unsigned int* idx = reinterpret_cast<const unsigned int*>(
          model.buffers[model.bufferViews[idxAccessor.bufferView].buffer].data.data()
          + model.bufferViews[idxAccessor.bufferView].byteOffset + idxAccessor.byteOffset
        );
        for (int i = 0; i < indexCount; i++) submesh.indices.push_back(idx[i]);
      }
    }

    mesh.submeshes.push_back(submesh);
  }

  return mesh;
}

static const float* getBufferData(const tinygltf::Model& model, int accessorIndex) {
  const tinygltf::Accessor& accessor = model.accessors[accessorIndex];
  const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
  const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

  return reinterpret_cast<const float*>(
    buffer.data.data() + bufferView.byteOffset + accessor.byteOffset
  );
}

static const unsigned short* getIndexData(const tinygltf::Model& model, int accessorIndex) {
  const tinygltf::Accessor& accessor = model.accessors[accessorIndex];
  const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
  const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

  return reinterpret_cast<const unsigned short*>(
    buffer.data.data() + bufferView.byteOffset + accessor.byteOffset
  );
}
