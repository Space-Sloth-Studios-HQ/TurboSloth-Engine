#pragma once

#include "AssetPool.h"
#include "Handle.h"
#include "ModelData.h"

#include <unordered_map>
#include <string>

namespace Momo::Assets {
class AssetRegistry
{
private:
    AssetPool<TextureData, TextureTag> textureAssets;
    AssetPool<Material, MaterialTag> materialAssets;
    AssetPool<Mesh, MeshTag> meshAssets;

    std::unordered_map<std::string, TextureHandle> texturesByPath;
public:
    // Add your asset registry methods here
    const TextureData& Get(TextureHandle handle) const { return textureAssets.Get(handle); }
    const Material& Get(MaterialHandle handle) const { return materialAssets.Get(handle); }
    const Mesh& Get(MeshHandle handle) const { return meshAssets.Get(handle); }

    void LoadModelData(const ModelData& modelData);
};

} // namespace Momo::Assets