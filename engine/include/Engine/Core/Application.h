#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <utility>
#include <memory>
#include "Engine/Window.h"
#include "Engine/Renderer/VulkanRenderer.h"

namespace Engine
{
    struct ApplicationSpecification 
    {
        std::string Name = "Untitled";
        WindowSpecification WindowSpec{};
    };

    class Layer 
    {
    public:
        virtual ~Layer() = default;
        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(float dt) {}
    };

    class Application {
    private:
        ApplicationSpecification m_Spec{};
        bool m_Running = true;
        bool m_IsShutdown = false;

        std::unique_ptr<IWindow> m_Window;
        VulkanRenderer m_Renderer; // Maybe this can be abstracted later

        std::vector<std::unique_ptr<Layer>> m_Layers;
    public:
        explicit Application(const ApplicationSpecification& spec);
        ~Application();

        template<typename TLayer, typename... Args>
        TLayer* PushLayer(Args&&... args)
        {
            auto layer = std::make_unique<TLayer>(std::forward<Args>(args)...);
            TLayer* raw = layer.get();
            m_Layers.emplace_back(std::move(layer));
            raw->OnAttach();
            return raw;
        }

        void Run();
        void Shutdown();
    };
}