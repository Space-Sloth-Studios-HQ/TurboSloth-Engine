#include "Momo/Assets/ModelLoader.h"
#include "Momo/Logging/Logger.h"
#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <string>

namespace Momo::Assets {
class FastGltfModelLoader : public IModelLoader {
private:
    fastgltf::math::fmat4x4 mapGLMToFastGltf(const glm::mat4 &mat) {
        fastgltf::math::fmat4x4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result[i][j] = mat[i][j];
            }
        }
        return result;
    }

    fastgltf::Parser parser;
    const std::string TAG = "FastGltfModelLoader";

public:
    FastGltfModelLoader() : parser(fastgltf::Parser()) {}
        
    std::optional<MeshData> LoadModel(const std::filesystem::path &path) override {
        // Implementation for loading a model using FastGltf goes here
        if (path.empty()) {
            LOG_ERROR(TAG, "Path is empty: {}", path.string());
            return std::nullopt;
        }

        auto model = fastgltf::GltfDataBuffer::FromPath(path);
        if (!model) {
            LOG_ERROR(TAG, "Failed to create GLTF data buffer from path: {}", path.string());
            return std::nullopt;
        }

        auto options = fastgltf::Options::LoadExternalBuffers | fastgltf::Options::LoadExternalImages;
        auto asset = parser.loadGltf(model.get(), path.parent_path(), options);
        if (!asset) {
            LOG_ERROR(TAG, "Failed to load GLTF asset from path: {}", path.string());
            return std::nullopt;
        }

        size_t sceneIndex = 0;
        // TODO: Identity for now, should be replaced with actual scene transformation from the entity
        fastgltf::math::fmat4x4 sceneTransform = mapGLMToFastGltf(glm::mat4(1.0f));

        MeshData res;
        fastgltf::iterateSceneNodes(asset.get(), sceneIndex, sceneTransform, [&](fastgltf::Node &node, fastgltf::math::fmat4x4 nodeTransform) {
            auto &gltf = asset.get();
            // Process each node here
            if (node.meshIndex.has_value()) {
                LOG_INFO(TAG, "Node has a mesh index: {}", node.meshIndex.value());
                for (const auto &primitive : gltf.meshes[node.meshIndex.value()].primitives) {
                    if (primitive.type != fastgltf::PrimitiveType::Triangles) {
                        LOG_WARN(TAG, "Unsupported primitive type: Primitive type is not triangles, skipping");
                        continue;
                    }
                    auto positionAttribute = primitive.findAttribute("POSITION");
                    if (positionAttribute == primitive.attributes.end()) {
                        LOG_WARN(TAG, "Primitive does not have POSITION attribute, skipping");
                        continue;
                    }
                    auto normalAttribute = primitive.findAttribute("NORMAL");
                    if (normalAttribute == primitive.attributes.end()) {
                        LOG_WARN(TAG, "Primitive does not have NORMAL attribute, skipping");
                        continue;
                    }
                    auto& posAccessor = gltf.accessors[positionAttribute->accessorIndex];
                    res.vertices.resize(posAccessor.count);

                    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(gltf, posAccessor,
                    [&](fastgltf::math::fvec3 pos, std::size_t index) {
                        res.vertices[index].position = glm::vec3(pos.x(), pos.y(), pos.z());
                    });

                    auto& normAccessor = gltf.accessors[normalAttribute->accessorIndex];
                    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(gltf, normAccessor,
                    [&](fastgltf::math::fvec3 norm, std::size_t index) {
                        res.vertices[index].normal = glm::vec3(norm.x(), norm.y(), norm.z());
                    });

                    auto& indexAccessorIdx = primitive.indicesAccessor;
                    if (!indexAccessorIdx.has_value()) {
                        LOG_INFO(TAG, "Primitive does not have an index accessor, skipping");
                        continue;
                    }
                    auto &indexAccessor = gltf.accessors[*indexAccessorIdx];
                    res.indices.resize(indexAccessor.count);
                    fastgltf::iterateAccessorWithIndex<std::uint32_t>(gltf, indexAccessor,
                    [&](std::uint32_t indexValue, std::size_t index) {
                        res.indices[index] = indexValue;
                    });
                }
            }
        });

        // Load the model using FastGltf here
        return res;
    }

}; 

// Factory
IModelLoader* IModelLoader::CreateGltfModelLoader()
{
    return new FastGltfModelLoader();
}
} // namespace Momo::Assets
