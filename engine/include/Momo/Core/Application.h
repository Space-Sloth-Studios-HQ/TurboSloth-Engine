#pragma once
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include "Momo/Window.h"
#include "Momo/Renderer/VulkanRenderer.h"
#include "Momo/Renderer/Camera.h"
#include "Momo/Assets/ModelLoader.h"

namespace Momo
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
        Renderer::VulkanMeshData LoadMesh(const std::filesystem::path &path);

        std::unique_ptr<IWindow> m_Window;
        Renderer::VulkanRenderer m_Renderer; // Maybe this can be abstracted later
        std::vector<std::unique_ptr<Layer>> m_Layers;
        std::unique_ptr<Assets::IModelLoader> m_ModelLoader;

        // Scene data
        std::optional<Camera> m_Camera;
        std::optional<Renderer::VulkanMeshData> m_Mesh; // TODO: Should be a full scene object as active scene
    };
}