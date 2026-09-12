#include "Engine/Core/Application.h"
#include <chrono>
#include <Engine/Logging/Logger.h>

namespace Engine
{
    Application::Application(const ApplicationSpecification& spec)
        : m_Spec(spec)
    {
        LOG_INFO("Engine", "Starting '{}' ({}x{})", m_Spec.Name, m_Spec.WindowSpec.Width, m_Spec.WindowSpec.Height);

        m_Window = std::unique_ptr<IWindow>(IWindow::Create(m_Spec.WindowSpec));
        m_Renderer.Init(*m_Window);
        // Temporary vertex buffer for hardcoded triangle vertices in the vertex shader
        std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},  // 0
            {{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},  // 1
            {{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},  // 2
            {{-0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}},  // 3
        };

        std::vector<uint16_t> indices = { 0, 1, 2,  2, 3, 0 };

        m_VertexBuffer = m_Renderer.CreateVertexBuffer(vertices);
        m_IndexBuffer = m_Renderer.CreateIndexBuffer(indices);
    }

    Application::~Application()
    {
        // Ensure shutdown is called even if the user forgot
        if (!m_IsShutdown)
        {
            LOG_WARN("Engine", "Shutdown() was not called explicitly. Calling now...");
            
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

            m_Renderer.RenderFrame(*m_VertexBuffer, *m_IndexBuffer);

            // crude temporary limiter so the console doesn't spam
            // std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

    void Application::Shutdown()
    {
        if (m_IsShutdown)
        {
            LOG_WARN("Engine", "Shutdown() called multiple times. Ignoring...");
            return;
        }

        LOG_INFO("Engine", "Shutting down...");

        // Shutdown renderer first (destroys Vulkan instance before GLFW terminates)
        m_Renderer.Shutdown();

        // Detach all layers before they are destroyed
        for (auto& layer : m_Layers)
        {
            layer->OnDetach();
        }

        m_IsShutdown = true;
        LOG_INFO("Engine", "Shutdown complete.");
    }
}