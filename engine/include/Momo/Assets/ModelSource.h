#pragma once

#include "Momo/Geometry/Vertex.h"
#include <vector>
#include <optional>
#include <cstdint>
#include <glm/glm.hpp>

/*
 * Source data classes from ModelLoader to be registered in the Registry
 **/
namespace Momo::Assets {
struct TextureSource {
    // Raw, tightly packed pixels: width * height * channels bytes.
    std::vector<uint8_t> pixels;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t channels = 0;

    bool IsEmpty() const { return pixels.empty(); }
};

struct MaterialSource {
    glm::vec4 baseColorFactor = glm::vec4(1.0f);
    // Index into ModelSource::textures, which mirrors the glTF image list.
    std::optional<uint32_t> baseColorTexture;
};

struct MeshSource {
    Geometry::MeshData geometry;
    glm::mat4 localTransform = glm::mat4(1.0f);
    // Index into ModelSource::materials, which mirrors the glTF material list.
    std::optional<uint32_t> materialIndex;
};

struct ModelSource {
    // meshes is one entry per drawn primitive; materials and textures are flat
    // file-level tables whose indices match the glTF material / image lists, so
    // primitives that share a material or image share the entry here too.
    std::vector<MeshSource> meshes;
    std::vector<MaterialSource> materials;
    std::vector<TextureSource> textures;
};
} // namespace Momo::Assets
