#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <utility>
#include <memory>

namespace Engine::Core
{
    struct WindowSpecification 
    {
        std::string Title = "Rocket Sloth";
        uint32_t Width = 1280;
        uint32_t Height = 720;
    };

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
    };
}