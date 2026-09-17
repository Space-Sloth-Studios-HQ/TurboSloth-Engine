#pragma once

#include <vector>
#include <optional>
#include <filesystem>
#include "Momo/Renderer/Vertex.h"


namespace Momo::Assets {
struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    glm::mat4 localTransform;
};

class IModelLoader {
public:
    virtual ~IModelLoader() = default;
    virtual std::optional<std::vector<MeshData>> LoadModel(const std::filesystem::path &path) = 0;

    // Factory
    static IModelLoader* CreateGltfModelLoader();
};
} // namespace Momo::Assets