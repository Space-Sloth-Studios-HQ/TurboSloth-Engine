#pragma once
#include "Momo/Geometry/Vertex.h"
#include "Handle.h"

namespace Momo::Assets {
struct TextureData {
    std::vector<uint8_t> pixelData; // Raw pixel data of the texture
    uint32_t width = 0;  // Width of the texture
    uint32_t height = 0; // Height of the texture
    uint32_t channels = 0; // Number of color channels in the texture (e.g., 3 for RGB, 4 for RGBA)

    // TODO: Support sampling parameters (e.g., filtering, wrapping)
    // Filtering mode (e.g., nearest, linear)
    // Wrapping mode (e.g., repeat, clamp to edge)
};

struct Material {
    glm::vec4 baseColorFactor; // Should be Material
    TextureHandle baseColorTextureHandle; // Handle to the base color texture for the material
};

struct Mesh {
    Geometry::MeshData meshData;
    glm::mat4 localTransform;
    MaterialHandle materialHandle; // Handle to the material used by this mesh
};

struct ModelData {
    std::vector<Mesh> meshes;
};
} // namespace Momo::Assets