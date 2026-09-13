#pragma once

namespace Engine {
namespace Input {
enum Key {
    W,
    A,
    S,
    D,
    Q,
    E,
    Space,
    Escape,
    Count
};

inline constexpr size_t KeyCount = static_cast<size_t>(Key::Count);
} // namespace Input
} // namespace Engine