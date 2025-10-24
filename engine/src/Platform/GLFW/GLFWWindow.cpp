#include "Engine/Window.h"
#include <cstdint>
#include <stdexcept>
#include <string>
#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Engine
{
    class GLFWWindow : public IWindow {
    public:
        explicit GLFWWindow(const WindowSpecification& spec) {
            if (s_InitCount == 0) {
                if (!glfwInit())
                    throw std::runtime_error("glfwInit failed");
                // (Optional) glfwSetErrorCallback([](int code, const char* desc){ std::cerr << "GLFW " << code << ": " << desc << "\n"; });
            }
            ++s_InitCount;

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // we’ll use Vulkan later
            m_Window = glfwCreateWindow(static_cast<int>(spec.Width),
                                        static_cast<int>(spec.Height),
                                        spec.Title.c_str(),
                                        nullptr, nullptr);
            if (!m_Window) {
                --s_InitCount;
                if (s_InitCount == 0) glfwTerminate();
                throw std::runtime_error("glfwCreateWindow failed");
            }

            m_Width  = spec.Width;
            m_Height = spec.Height;

            // Hook resize if you want
            glfwSetWindowUserPointer(m_Window, this);
            glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* win, int w, int h){
                auto* self = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(win));
                self->m_Width  = static_cast<std::uint32_t>(w);
                self->m_Height = static_cast<std::uint32_t>(h);
            });
        }

        ~GLFWWindow() override {
            if (m_Window) {
                glfwDestroyWindow(m_Window);
                m_Window = nullptr;
            }
            --s_InitCount;
            if (s_InitCount == 0)
                glfwTerminate();
        }

        void PollEvents() override { glfwPollEvents(); }
        bool ShouldClose() const override { return glfwWindowShouldClose(m_Window); }

        void* GetNativeHandle() const override { return m_Window; }
        std::uint32_t GetWidth()  const override { return m_Width; }
        std::uint32_t GetHeight() const override { return m_Height; }

    private:
        GLFWwindow* m_Window = nullptr;
        std::uint32_t m_Width = 0, m_Height = 0;

        static inline int s_InitCount = 0;
    };

    // Factory
    IWindow* IWindow::Create(const WindowSpecification& spec) {
        return new GLFWWindow(spec);
    }
}
