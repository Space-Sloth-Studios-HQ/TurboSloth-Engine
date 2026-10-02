#pragma once

#include <stdexcept>
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
        assets.push_back(std::move(assetData));
        return Handle<Tag>{static_cast<uint32_t>(assets.size() - 1)};
    }

    const T& Get(Handle<Tag> handle) const {
        if (!Has(handle))
            throw std::runtime_error("Invalid handle or asset does not exist.");

        return *assets[handle.id];
    }

    [[nodiscard]] const T* TryGet(Handle<Tag> handle) const {
        if (!Has(handle))
            return nullptr;

        return &*assets[handle.id];
    }

    void Remove(Handle<Tag> handle) {
        if (!Has(handle))
            throw std::runtime_error("Invalid handle or asset does not exist.");

        assets[handle.id].reset();
    }

    bool Has(Handle<Tag> handle) const {
        return handle.IsValid() && handle.id < assets.size() && assets[handle.id].has_value();
    }

    size_t GetAssetCount() const {
        return assets.size();
    }
};
} // namespace Momo::Assets
