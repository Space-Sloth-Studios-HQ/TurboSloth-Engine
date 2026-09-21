#pragma once

#include <vector>
#include <optional>
#include "Handle.h"

namespace Momo::Assets {


template <typename T, typename Tag>
class AssetPool {
private:
    std::vector<std::optional<T>> assets;

public:
    AssetPool() = default;
    ~AssetPool() = default;

    Handle<Tag> Add(T assetData) {
        assets.push_back(assetData);
        return Handle<Tag>{static_cast<uint32_t>(assets.size() - 1)};
    }

    const T& Get(Handle<Tag> handle) const {
        return *assets[handle.id];
    }

    void Remove(Handle<Tag> handle) {
        assets[handle.id].reset();
    }

    bool Has(Handle<Tag> handle) const {
        return handle.IsValid() && assets[handle.id].has_value();
    }

    size_t GetAssetCount() const {
        return assets.size();
    }
};
} // namespace Momo::Assets