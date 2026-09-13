#pragma once
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include "Engine/Window.h"
#include "Engine/Renderer/VulkanRenderer.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Input/InputState.h"

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
    private:
        ApplicationSpecification m_Spec{};
        bool m_Running = true;
        bool m_IsShutdown = false;

        std::unique_ptr<IWindow> m_Window;
        VulkanRenderer m_Renderer; // Maybe this can be abstracted later

        std::optional<AllocatedBuffer> m_VertexBuffer;
        std::optional<AllocatedBuffer> m_IndexBuffer;
        std::optional<Camera> m_Camera;

        std::vector<std::unique_ptr<Layer>> m_Layers;
    };
}