#pragma once

#include "AssetPool.h"
#include "Handle.h"
#include "ModelData.h"
#include "ModelSource.h"

#include <unordered_map>
#include <string>

namespace Momo::Assets {
class AssetRegistry
{
private:
    AssetPool<TextureData, TextureTag> textureAssets;
    AssetPool<Material, MaterialTag> materialAssets;
    AssetPool<Mesh, MeshTag> meshAssets;
    AssetPool<Model, ModelTag> modelAssets;

    std::unordered_map<std::string, TextureHandle> texturesByPath;
public:
    // Add your asset registry methods here
    const TextureData& Get(TextureHandle handle) const { return textureAssets.Get(handle); }
    const Material& Get(MaterialHandle handle) const { return materialAssets.Get(handle); }
    const Mesh& Get(MeshHandle handle) const { return meshAssets.Get(handle); }
    const Model& Get(ModelHandle handle) const { return modelAssets.Get(handle); }

    // Registers everything a loader produced and hands back the model. This is
    // the only place local ModelSource indices become engine-wide handles:
    // textures first, then materials that reference them, then meshes.
    ModelHandle RegisterModel(const ModelSource& source);
};

} // namespace Momo::Assets
