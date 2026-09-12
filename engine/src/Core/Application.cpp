#include "Engine/Core/Application.h"
#include <chrono>
#include <Engine/Logging/Logger.h>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Vulkan depth [0, 1] range
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
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
            // front (+Z) - red
            {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.2f, 0.2f}, { 0.0f,  0.0f,  1.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.2f, 0.2f}, { 0.0f,  0.0f,  1.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.2f, 0.2f}, { 0.0f,  0.0f,  1.0f}},
            {{-0.5f,  0.5f,  0.5f}, {1.0f, 0.2f, 0.2f}, { 0.0f,  0.0f,  1.0f}},
            // right (+X) - green
            {{ 0.5f, -0.5f,  0.5f}, {0.2f, 1.0f, 0.2f}, { 1.0f,  0.0f,  0.0f}},
            {{ 0.5f, -0.5f, -0.5f}, {0.2f, 1.0f, 0.2f}, { 1.0f,  0.0f,  0.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {0.2f, 1.0f, 0.2f}, { 1.0f,  0.0f,  0.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {0.2f, 1.0f, 0.2f}, { 1.0f,  0.0f,  0.0f}},
            // back (-Z) - blue
            {{ 0.5f, -0.5f, -0.5f}, {0.2f, 0.2f, 1.0f}, { 0.0f,  0.0f, -1.0f}},
            {{-0.5f, -0.5f, -0.5f}, {0.2f, 0.2f, 1.0f}, { 0.0f,  0.0f, -1.0f}},
            {{-0.5f,  0.5f, -0.5f}, {0.2f, 0.2f, 1.0f}, { 0.0f,  0.0f, -1.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {0.2f, 0.2f, 1.0f}, { 0.0f,  0.0f, -1.0f}},
            // left (-X) - yellow
            {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 0.2f}, {-1.0f,  0.0f,  0.0f}},
            {{-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 0.2f}, {-1.0f,  0.0f,  0.0f}},
            {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 0.2f}, {-1.0f,  0.0f,  0.0f}},
            {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 0.2f}, {-1.0f,  0.0f,  0.0f}},
            // top (+Y) - magenta
            {{-0.5f,  0.5f,  0.5f}, {1.0f, 0.2f, 1.0f}, { 0.0f,  1.0f,  0.0f}},
            {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.2f, 1.0f}, { 0.0f,  1.0f,  0.0f}},
            {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.2f, 1.0f}, { 0.0f,  1.0f,  0.0f}},
            {{-0.5f,  0.5f, -0.5f}, {1.0f, 0.2f, 1.0f}, { 0.0f,  1.0f,  0.0f}},
            // bottom (-Y) - cyan
            {{-0.5f, -0.5f, -0.5f}, {0.2f, 1.0f, 1.0f}, { 0.0f, -1.0f,  0.0f}},
            {{ 0.5f, -0.5f, -0.5f}, {0.2f, 1.0f, 1.0f}, { 0.0f, -1.0f,  0.0f}},
            {{ 0.5f, -0.5f,  0.5f}, {0.2f, 1.0f, 1.0f}, { 0.0f, -1.0f,  0.0f}},
            {{-0.5f, -0.5f,  0.5f}, {0.2f, 1.0f, 1.0f}, { 0.0f, -1.0f,  0.0f}},
        };

        std::vector<uint16_t> indices = {
            0,  1,  2,   2,  3,  0,   // front
            4,  5,  6,   6,  7,  4,   // right
            8,  9, 10,  10, 11,  8,   // back
            12, 13, 14,  14, 15, 12,   // left
            16, 17, 18,  18, 19, 16,   // top
            20, 21, 22,  22, 23, 20,   // bottom
        };


        m_VertexBuffer = m_Renderer.CreateVertexBuffer(vertices);
        m_IndexBuffer = m_Renderer.CreateIndexBuffer(indices);
        m_Camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
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

        uint32_t cubeRotationSpeed = 1; // radians per second


        while (m_Running && !m_Window->ShouldClose())
        {
            auto now = clock::now();
            float dt = std::chrono::duration<float>(now - last).count();
            float totalTime = std::chrono::duration<float>(now.time_since_epoch()).count();
            LOG_INFO("Engine", "Frame time: {}", dt);
            last = now;

            m_Window->PollEvents();

            for (auto& layer : m_Layers)
                layer->OnUpdate(dt);

            glm::vec3 rotationAxis = glm::normalize(glm::vec3(0.5f, 1.0f, 0.0f));
            glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), totalTime * cubeRotationSpeed, rotationAxis);
            LOG_INFO("Engine", "Model matrix: {}", glm::to_string(modelMatrix));

            m_Renderer.RenderFrame(*m_VertexBuffer, *m_IndexBuffer, m_Camera->GetViewMatrix(), modelMatrix);

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