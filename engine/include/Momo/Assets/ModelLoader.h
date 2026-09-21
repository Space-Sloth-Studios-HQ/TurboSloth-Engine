#pragma once

#include <optional>
#include <filesystem>
#include "Momo/Assets/ModelSource.h"


namespace Momo::Assets {
class IModelLoader {
public:
    virtual ~IModelLoader() = default;
    virtual std::optional<Assets::ModelSource> LoadModel(const std::filesystem::path &path) = 0;

    // Factory
    static IModelLoader* CreateGltfModelLoader();
};
} // namespace Momo::Assets
