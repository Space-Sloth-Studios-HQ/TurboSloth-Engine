#pragma once
#include <cstdint>
#include <limits>

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
