#pragma once

#include <vector>
#include "Momo/Assets/Handle.h"

namespace Momo {
class Scene
{
public:
    Scene() = default;
    ~Scene() = default;

    void AddModel(const Assets::ModelHandle& model) {
        m_Models.push_back(model);
    }
    const std::vector<Assets::ModelHandle>& GetModels() const {
        return m_Models;
    }
private:
    std::vector<Assets::ModelHandle> m_Models;
};
} // namespace Momo