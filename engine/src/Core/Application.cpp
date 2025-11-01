#include "Engine/Core/Application.h"
#include <chrono>
#include <thread>
#include <iostream>

namespace Engine
{
    Application::Application(const ApplicationSpecification& spec)
        : m_Spec(spec)
    {
        std::cout << "[Engine] Starting '" << m_Spec.Name
                  << "' (" << m_Spec.WindowSpec.Width << "x"
                  << m_Spec.WindowSpec.Height << ")\n";

        m_Window = std::unique_ptr<IWindow>(IWindow::Create(m_Spec.WindowSpec));
        m_Renderer.Init(*m_Window);
    }

    Application::~Application()
    {
        // Ensure shutdown is called even if the user forgot
        if (!m_IsShutdown)
        {
            std::cout << "[Engine] Warning: Shutdown() was not called explicitly. Calling now...\n";
            Shutdown();
        }
    }

    void Application::Run()
    {
        using clock = std::chrono::steady_clock;
        auto last = clock::now();

        while (m_Running && !m_Window->ShouldClose())
        {
            auto now = clock::now();
            float dt = std::chrono::duration<float>(now - last).count();
            last = now;

            m_Window->PollEvents();

            for (auto& layer : m_Layers)
                layer->OnUpdate(dt);

            // crude temporary limiter so the console doesn't spam
            // std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

    void Application::Shutdown()
    {
        if (m_IsShutdown)
        {
            std::cout << "[Engine] Warning: Shutdown() called multiple times. Ignoring...\n";
            return;
        }

        std::cout << "[Engine] Shutting down...\n";

        // Shutdown renderer first (destroys Vulkan instance before GLFW terminates)
        m_Renderer.Shutdown();

        // Detach all layers before they are destroyed
        for (auto& layer : m_Layers)
        {
            layer->OnDetach();
        }

        m_IsShutdown = true;
        std::cout << "[Engine] Shutdown complete.\n";
    }
}