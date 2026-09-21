#include "Momo/Assets/AssetRegistry.h"

namespace Momo::Assets {
namespace {
TextureData ToTextureData(const TextureSource& source) {
    TextureData textureData;
    textureData.pixelData = source.pixels;
    textureData.width = source.width;
    textureData.height = source.height;
    textureData.channels = source.channels;
    return textureData;
}
} // namespace

ModelHandle AssetRegistry::RegisterModel(const ModelSource& source) {
    // Local index -> handle lookup tables. Slots the loader left empty stay
    // invalid, so a reference to a failed decode is detectable rather than
    // silently pointing at the wrong asset.
    std::vector<TextureHandle> textureHandles(source.textures.size());
    for (size_t i = 0; i < source.textures.size(); ++i) {
        if (!source.textures[i].IsEmpty()) {
            textureHandles[i] = textureAssets.Add(ToTextureData(source.textures[i]));
        }
    }

    std::vector<MaterialHandle> materialHandles(source.materials.size());
    for (size_t i = 0; i < source.materials.size(); ++i) {
        Material material;
        material.baseColorFactor = source.materials[i].baseColorFactor;
        if (auto textureIndex = source.materials[i].baseColorTexture) {
            material.baseColorTextureHandle = textureHandles[*textureIndex];
        }
        materialHandles[i] = materialAssets.Add(material);
    }

    Model model;
    model.meshes.reserve(source.meshes.size());
    for (const auto& meshSource : source.meshes) {
        Mesh mesh;
        mesh.meshData = meshSource.geometry;
        mesh.localTransform = meshSource.localTransform;
        if (auto materialIndex = meshSource.materialIndex) {
            mesh.materialHandle = materialHandles[*materialIndex];
        }
        model.meshes.push_back(meshAssets.Add(std::move(mesh)));
    }

    return modelAssets.Add(std::move(model));
}
} // namespace Momo::Assets
