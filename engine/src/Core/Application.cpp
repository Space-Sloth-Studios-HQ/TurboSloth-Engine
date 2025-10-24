#include "Engine/Core/Application.h"
#include <chrono>
#include <thread>
#include <iostream>

namespace Engine::Core
{
    Application::Application(const ApplicationSpecification& spec)
        : m_Spec(spec)
    {
        // TODO: Create actual window from m_spec.WindowSpec
        std::cout << "[Engine] Starting '" << m_Spec.Name
                  << "' (" << m_Spec.WindowSpec.Width << "x"
                  << m_Spec.WindowSpec.Height << ")\n";
    }

    Application::~Application()
    {
        // Destroy window, layers, etc
    }

    void Application::Run()
    {
        using clock = std::chrono::steady_clock;
        auto last = clock::now();

        while (m_Running)
        {
            auto now = clock::now();
            float dt = std::chrono::duration<float>(now - last).count();
            last = now;

            // Poll OS events here (GLFW/Win32 later)

            // for (auto& layer : m_Layers)
            //     layer->OnUpdate(dt);

            // crude temporary limiter so the console doesn't spam
            // std::this_thread::sleep_for(std::chrono::milliseconds(16));

            // temp exit condition placeholder
            if (false) m_Running = false;
        }        
    }
}