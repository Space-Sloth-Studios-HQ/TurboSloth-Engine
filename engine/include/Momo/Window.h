#pragma once
#include <string>
#include <cstdint>
#include "Momo/Input/InputState.h"

namespace Momo
{
    struct WindowSpecification {
        std::string Title = "Momo Window";
        uint32_t Width;
        uint32_t Height;
    };

    class IWindow {
    public:
        virtual ~IWindow() = default;
        virtual Input::InputState ReadInput() = 0;
        virtual void PollEvents() = 0;
        virtual bool ShouldClose() const = 0;

        virtual void* GetNativeHandle() const = 0; // return GLFWwindow*
        virtual std::uint32_t GetWidth() const = 0;
        virtual std::uint32_t GetHeight() const = 0;

        static IWindow* Create(const WindowSpecification& spec); // Factory method
    };
}