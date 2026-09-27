#pragma once

#include "Momo/Assets/Handle.h"
#include "Momo/Assets/AssetRegistry.h"
#include "Momo/Renderer/VulkanRenderer.h"
#include <unordered_map>

namespace Momo {
template <typename Tag> struct GPUResourceTraits;
template <> struct GPUResourceTraits<Assets::TextureTag> {
    using GPUType = Renderer::AllocatedImage;
    static GPUType Create(const Assets::TextureData& textureData, Renderer::VulkanRenderer& renderer) {
        return renderer.CreateAndSubmitTextureImage(textureData);
    }
};

template <typename Tag> struct GPUResourceTraits;
template <> struct GPUResourceTraits<Assets::MeshTag> {
    using GPUType = Renderer::GPUMesh;
    static GPUType Create(const Assets::Mesh& mesh, Renderer::VulkanRenderer& renderer) {
        return Renderer::GPUMesh {
                    .vertexBuffer = renderer.CreateVertexBuffer(mesh.meshData.vertices),
                    .indexBuffer = renderer.CreateIndexBuffer(mesh.meshData.indices),
                };
    }
};

template <typename Tag>
class GPUCache
{
public:
    using GPUType = typename GPUResourceTraits<Tag>::GPUType;

    GPUCache(const Assets::AssetRegistry& assetRegistry, Renderer::VulkanRenderer& renderer) 
        : m_AssetRegistry(assetRegistry), m_Renderer(renderer) {}

    ~GPUCache() = default;

    const GPUType& GetOrCreate(Assets::Handle<Tag> handle) {
        auto it = m_Cache.find(handle);
        if (it == m_Cache.end()) {
            it = m_Cache.emplace(handle, GPUResourceTraits<Tag>::Create(m_AssetRegistry.Get(handle), m_Renderer)).first;
        }
        return it->second;
    }
private:
    std::unordered_map<Assets::Handle<Tag>, GPUType> m_Cache;
    const Assets::AssetRegistry& m_AssetRegistry;
    Renderer::VulkanRenderer& m_Renderer;
};

using TextureGPUCache = GPUCache<Assets::TextureTag>;
using MeshGPUCache = GPUCache<Assets::MeshTag>;

} // namespace Momo