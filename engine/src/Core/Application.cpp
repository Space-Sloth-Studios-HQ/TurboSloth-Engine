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
        // Destroy window, layers, etc
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
}