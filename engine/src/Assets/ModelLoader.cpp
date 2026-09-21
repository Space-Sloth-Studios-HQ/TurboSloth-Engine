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

    std::optional<TextureSource> LoadTextureSourceFromFile(const std::filesystem::path &path) {
        return std::make_optional<TextureSource>();
    }
    std::optional<TextureSource> LoadTextureSourceFromBufferView(const fastgltf::sources::BufferView &bufferView) {
        return std::make_optional<TextureSource>();
    }
    std::optional<TextureSource> LoadTextureSourceFromArray(const fastgltf::sources::Array &arraySource) {
        return std::make_optional<TextureSource>();
    }

    // Decodes one glTF image. Images are file-level, so this runs once per
    // image no matter how many primitives end up referencing it.
    std::optional<TextureSource> LoadTextureSource(const fastgltf::Image &image,
                                                   const std::filesystem::path &baseDir) {
        return std::visit<std::optional<TextureSource>>(fastgltf::visitor{
            [&](const fastgltf::sources::URI &uriSource) {
                // TODO: Implement actual image loading from the path
                return LoadTextureSourceFromFile(baseDir / std::filesystem::path(uriSource.uri.string()));
            },
            [&](const fastgltf::sources::BufferView &bufferView) {
                // TODO: Implement actual image loading from the buffer view
                return LoadTextureSourceFromBufferView(bufferView);
            },
            [&](const fastgltf::sources::Array &arraySource) {
                // TODO: Implement actual image loading from the array source
                return LoadTextureSourceFromArray(arraySource);
            },
            [&](auto&) -> std::optional<TextureSource> {
                LOG_WARN(TAG, "Unsupported image source");
                return std::nullopt;
            }
        }, image.data);
    }

    // Walks material -> texture -> image to find the image backing the base
    // colour, so materials can reference ModelSource::textures by index.
    std::optional<uint32_t> FindBaseColorImageIndex(const fastgltf::Asset &gltf,
                                                    const fastgltf::Material &material) {
        const auto &baseColorTexture = material.pbrData.baseColorTexture;
        if (!baseColorTexture.has_value()) {
            return std::nullopt;
        }

        if (baseColorTexture->texCoordIndex != 0) {
            LOG_WARN(TAG, "Base color texture uses TEXCOORD_{}, only TEXCOORD_0 is supported",
                     baseColorTexture->texCoordIndex);
        }

        const auto &texture = gltf.textures[baseColorTexture->textureIndex];
        if (!texture.imageIndex.has_value()) {
            LOG_WARN(TAG, "Texture has no associated image");
            return std::nullopt;
        }
        // TODO: texture.samplerIndex is dropped here; revisit when TextureData
        // grows sampling parameters.
        return static_cast<uint32_t>(*texture.imageIndex);
    }

    fastgltf::Parser parser;
    const std::string TAG = "FastGltfModelLoader";

public:
    FastGltfModelLoader() : parser(fastgltf::Parser()) {}
        
    std::optional<Assets::ModelSource> LoadModel(const std::filesystem::path &path) override {
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

        std::vector<MeshSource> meshSources;
        std::vector<TextureSource> texSources;
        std::vector<MaterialSource> matSources;

        // Pass 1: images. Indices line up 1:1 with the glTF image list, so a
        // failed decode leaves an empty slot instead of shifting every index
        // after it.
        texSources.resize(asset->images.size());
        for (size_t i = 0; i < asset->images.size(); ++i) {
            if (auto textureSource = LoadTextureSource(asset->images[i], path.parent_path())) {
                texSources[i] = std::move(*textureSource);
            } else {
                LOG_WARN(TAG, "Failed to load image at index: {}", i);
            }
        }

        // Pass 2: materials, also 1:1 with the glTF material list.
        matSources.resize(asset->materials.size());
        for (size_t i = 0; i < asset->materials.size(); ++i) {
            const auto &material = asset->materials[i];
            matSources[i].baseColorFactor = glm::vec4(
                material.pbrData.baseColorFactor[0],
                material.pbrData.baseColorFactor[1],
                material.pbrData.baseColorFactor[2],
                material.pbrData.baseColorFactor[3]
            );
            matSources[i].baseColorTexture = FindBaseColorImageIndex(asset.get(), material);
        }

        // Pass 3: geometry per primitive. Materials and images are already
        // loaded, so this only records the index and never bails out early.
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

                    Assets::MeshSource meshSource = Assets::MeshSource();
                    Geometry::MeshData meshData = Geometry::MeshData();
                    meshSource.localTransform = mapFastGltfToGLM(nodeTransform);
                    auto& posAccessor = gltf.accessors[positionAttribute->accessorIndex];
                    meshData.vertices.resize(posAccessor.count);

                    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(gltf, posAccessor,
                    [&](fastgltf::math::fvec3 pos, std::size_t index) {
                        meshData.vertices[index].position = glm::vec3(pos.x(), pos.y(), pos.z());
                    });

                    auto& normAccessor = gltf.accessors[normalAttribute->accessorIndex];
                    fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(gltf, normAccessor,
                    [&](fastgltf::math::fvec3 norm, std::size_t index) {
                        meshData.vertices[index].normal = glm::vec3(norm.x(), norm.y(), norm.z());
                    });

                    auto& indexAccessorIdx = primitive.indicesAccessor;
                    if (indexAccessorIdx.has_value()) {
                        auto &indexAccessor = gltf.accessors[*indexAccessorIdx];
                        meshData.indices.resize(indexAccessor.count);
                        fastgltf::iterateAccessorWithIndex<std::uint32_t>(gltf, indexAccessor,
                        [&](std::uint32_t indexValue, std::size_t index) {
                            meshData.indices[index] = indexValue;
                        });
                    }
                    // TODO: Support other TEXCOORDs
                    auto texCoordAttribute = primitive.findAttribute("TEXCOORD_0");
                    if (texCoordAttribute != primitive.attributes.end()) {
                        auto &texCoordAccessor = gltf.accessors[texCoordAttribute->accessorIndex];
                        fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(gltf, texCoordAccessor,
                        [&](fastgltf::math::fvec2 uv, std::size_t index) {
                            meshData.vertices[index].texCoord = glm::vec2(uv.x(), uv.y());
                        });
                    }

                    if (primitive.materialIndex.has_value()) {
                        meshSource.materialIndex = static_cast<uint32_t>(*primitive.materialIndex);
                    }

                    meshSource.geometry = std::move(meshData);
                    meshSources.push_back(std::move(meshSource));
                }
            }
        });

        Assets::ModelSource modelSource = Assets::ModelSource();
        modelSource.meshes = std::move(meshSources);
        modelSource.materials = std::move(matSources);
        modelSource.textures = std::move(texSources);

        // Load the model using FastGltf here
        return modelSource;
    }

}; 

// Factory
IModelLoader* IModelLoader::CreateGltfModelLoader()
{
    return new FastGltfModelLoader();
}
} // namespace Momo::Assets
