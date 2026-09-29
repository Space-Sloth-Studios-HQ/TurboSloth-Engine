#include "Momo/Assets/AssetRegistry.h"

#include <utility>

namespace Momo::Assets {
namespace {
TextureData ToTextureData(TextureSource&& source) {
    TextureData textureData;
    textureData.pixelData = std::move(source.pixels);
    textureData.width = source.width;
    textureData.height = source.height;
    textureData.channels = source.channels;
    return textureData;
}

Material ToMaterial(const MaterialSource& source, const TextureHandle textureHandle) {
    Material material;
    material.baseColorFactor = source.baseColorFactor;
    material.alphaMode = source.alphaMode;
    material.alphaCutoff = source.alphaCutoff;
    material.baseColorTextureHandle = textureHandle;
    return material;
}

Mesh ToMesh(MeshSource&& source, const MaterialHandle materialHandle) {
    Mesh mesh;
    mesh.meshData = std::move(source.geometry);
    mesh.localTransform = source.localTransform;
    mesh.materialHandle = materialHandle;
    return mesh;
}
} // namespace

ModelHandle AssetRegistry::RegisterModel(ModelSource&& source) {
    // Local index -> handle lookup tables. Slots the loader left empty stay
    // invalid, so a reference to a failed decode is detectable rather than
    // silently pointing at the wrong asset.
    std::vector<TextureHandle> textureHandles(source.textures.size());
    for (size_t i = 0; i < source.textures.size(); ++i) {
        if (!source.textures[i].IsEmpty()) {
            textureHandles[i] = textureAssets.Add(ToTextureData(std::move(source.textures[i])));
        }
    }

    std::vector<MaterialHandle> materialHandles(source.materials.size());
    for (size_t i = 0; i < source.materials.size(); ++i) {
        // TODO: Consider using a default texture or logging a warning when a material has no base color texture.
        const auto &src = source.materials[i];
        const auto textureHandle = src.baseColorTexture ? textureHandles[*src.baseColorTexture] : TextureHandle{};
        materialHandles[i] = materialAssets.Add(ToMaterial(source.materials[i], textureHandle));
    }

    Model model;
    model.meshes.reserve(source.meshes.size());
    for (auto& meshSource : source.meshes) {
        // TODO: Consider using a default material or logging a warning when a mesh has no material.
        const auto materialHandle = meshSource.materialIndex ? materialHandles[*meshSource.materialIndex] : MaterialHandle{};
        Mesh mesh = ToMesh(std::move(meshSource), materialHandle);
        model.meshes.push_back(meshAssets.Add(std::move(mesh)));
    }

    return modelAssets.Add(std::move(model));
}
} // namespace Momo::Assets
