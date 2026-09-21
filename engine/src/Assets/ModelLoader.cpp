#include "Momo/Assets/ModelLoader.h"
#include "Momo/Logging/Logger.h"
#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <string>
#include <variant>

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

    glm::mat4 mapFastGltfToGLM(const fastgltf::math::fmat4x4 &mat) {
        glm::mat4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result[i][j] = mat[i][j];
            }
        }
        return result;
    }

    std::optional<TextureData> LoadTextureDataFromFile(const std::filesystem::path &path) {
        return std::make_optional<TextureData>();
    }
    std::optional<TextureData> LoadTextureDataFromBufferView(const fastgltf::sources::BufferView &bufferView) {
        return std::make_optional<TextureData>();
    }
    std::optional<TextureData> LoadTextureDataFromArray(const fastgltf::sources::Array &arraySource) {
        return std::make_optional<TextureData>();
    }

    fastgltf::Parser parser;
    const std::string TAG = "FastGltfModelLoader";

public:
    FastGltfModelLoader() : parser(fastgltf::Parser()) {}
        
    std::optional<Assets::ModelData> LoadModel(const std::filesystem::path &path) override {
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

        size_t sceneIndex = asset->defaultScene.value_or(0);
        // TODO: Identity for now, should be replaced with actual scene transformation from the entity
        fastgltf::math::fmat4x4 sceneTransform = mapGLMToFastGltf(glm::mat4(1.0f));

        std::vector<Mesh> resVec;
        std::vector<TextureData> resTextures;

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

                    Geometry::MeshData res = Geometry::MeshData();
                    res.localTransform = mapFastGltfToGLM(nodeTransform);
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
                    if (indexAccessorIdx.has_value()) {
                        auto &indexAccessor = gltf.accessors[*indexAccessorIdx];
                        res.indices.resize(indexAccessor.count);
                        fastgltf::iterateAccessorWithIndex<std::uint32_t>(gltf, indexAccessor,
                        [&](std::uint32_t indexValue, std::size_t index) {
                            res.indices[index] = indexValue;
                        });
                    }

                    auto materialIndex = primitive.materialIndex;
                    if (materialIndex.has_value()) {
                        auto &material = gltf.materials[*materialIndex];
                        auto resMaterial = Material();
                        resMaterial.baseColorFactor = glm::vec4(
                            material.pbrData.baseColorFactor[0],
                            material.pbrData.baseColorFactor[1],
                            material.pbrData.baseColorFactor[2],
                            material.pbrData.baseColorFactor[3]
                        );

                        auto &textureInfoIdx = material.pbrData.baseColorTexture;
                        if (!textureInfoIdx.has_value()) {
                            LOG_WARN(TAG, "Material has no base color texture");
                            continue;
                        }
                        auto &textureInfo = *textureInfoIdx;
                        auto texture = gltf.textures[textureInfo.textureIndex];
                        auto textureImageIndex = texture.imageIndex;
                        if (!textureImageIndex.has_value()) {
                            LOG_WARN(TAG, "Texture has no associated image");
                            continue;
                        }

                        // open texture image and fill resTextureData
                        auto &image = gltf.images[*textureImageIndex];

                        auto textureData = std::visit<std::optional<TextureData>>(fastgltf::visitor{
                            [&](const fastgltf::sources::URI &uriSource) {
                                // Load the image from the URI
                                std::string_view path = uriSource.uri.string();
                                // TODO: Implement actual image loading from the path
                                return LoadTextureDataFromFile(path);
                            },
                            [&](const fastgltf::sources::BufferView &bufferView) {
                                // Load the image from the buffer view
                                // TODO: Implement actual image loading from the buffer view
                                return LoadTextureDataFromBufferView(bufferView);
                            },
                            [&](const fastgltf::sources::Array &arraySource) {
                                // Load the image from the array source
                                // TODO: Implement actual image loading from the array source
                                return LoadTextureDataFromArray(arraySource);
                            },
                            [](auto) -> std::optional<TextureData> {
                                // Handle unsupported image sources
                                LOG_WARN("ModelLoader", "Unsupported image source");
                                return std::nullopt;
                            }
                        }, image.data);
                        
                        // Maybe should go to a texture handler to get a unique handle for the texture
                        if (textureData.has_value()) {
                            resTextures.push_back(*textureData);
                        }

                        resMaterial.baseColorTextureHandle = static_cast<uint32_t>(resTextures.size() - 1);
                    }

                    resVec.push_back(std::move(res));
                }
            }
        });
        Assets::ModelData result = Assets::ModelData();
        result.meshes = std::move(resVec);
        result.materials = std::move(resMaterials);

        // Load the model using FastGltf here
        return result;
    }

}; 

// Factory
IModelLoader* IModelLoader::CreateGltfModelLoader()
{
    return new FastGltfModelLoader();
}
} // namespace Momo::Assets
