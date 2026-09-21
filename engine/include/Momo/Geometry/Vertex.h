#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Momo {
namespace Geometry {
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;

};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};
} // namespace Geometry
} // namespace Momo