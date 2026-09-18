#pragma once

#include <vector>
#include <optional>
#include <filesystem>
#include "Geometry/Vertex.h"


namespace Momo::Assets {
class IModelLoader {
public:
    virtual ~IModelLoader() = default;
    virtual std::optional<std::vector<Momo::Geometry::MeshData>> LoadModel(const std::filesystem::path &path) = 0;

    // Factory
    static IModelLoader* CreateGltfModelLoader();
};
} // namespace Momo::Assets