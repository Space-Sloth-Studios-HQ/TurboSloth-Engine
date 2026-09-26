#pragma once
#include <cstdint>
#include <limits>
#include <functional>

namespace Momo::Assets {
template <typename Tag>
struct Handle { 
    static constexpr uint32_t Invalid = std::numeric_limits<uint32_t>::max();

    uint32_t id = Invalid;

    bool IsValid() const { return id != Invalid; }
    bool operator==(const Handle& other) const { return id == other.id; }
};

using TextureHandle     = Handle<struct TextureTag>;
using MaterialHandle    = Handle<struct MaterialTag>;
using MeshHandle        = Handle<struct MeshTag>;
using ModelHandle       = Handle<struct ModelTag>;

} // namespace Momo::Assets

namespace std {
template <typename Tag>
struct hash<Momo::Assets::Handle<Tag>> {
    std::size_t operator()(const Momo::Assets::Handle<Tag>& handle) const noexcept {
        return std::hash<uint32_t>{}(handle.id);
    }
};
} // namespace std
