#include "Momo/Core/Application.h"
#include "Momo/Renderer/VulkanMeshData.h"
#include <chrono>
#include <Momo/Logging/Logger.h>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Vulkan depth [0, 1] range
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
namespace Momo
{
    Application::Application(const ApplicationSpecification& spec)
        : m_Spec(spec)
    {
        LOG_INFO("Momo", "Starting '{}' ({}x{})", m_Spec.Name, m_Spec.WindowSpec.Width, m_Spec.WindowSpec.Height);
        m_Window = std::unique_ptr<IWindow>(IWindow::Create(m_Spec.WindowSpec));
        m_Renderer.Init(*m_Window);
        m_ModelLoader = std::unique_ptr<Assets::IModelLoader>(Assets::IModelLoader::CreateGltfModelLoader());

        // Scene loading
        // m_Mesh = LoadMesh("Assets/Models/Panko/PANKO_Rigged.glb");
        m_Mesh = LoadMesh("Assets/Models/Duck/Duck.gltf");
        m_Camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
    }

    Application::~Application()
    {
        // Ensure shutdown is called even if the user forgot
        if (!m_IsShutdown)
        {
            LOG_WARN("Momo", "Shutdown() was not called explicitly. Calling now...");
            
            Shutdown();
        }
    }

    void Application::Run()
    {
        using clock = std::chrono::steady_clock;
        auto last = clock::now();

        float totalTime = 0.0f;
        float cubeRotationSpeed = 1.0f; // radians per second


        while (m_Running && !m_Window->ShouldClose())
        {
            auto now = clock::now();
            float dt = std::chrono::duration<float>(now - last).count();
            totalTime += dt;
            LOG_INFO("Momo", "Frame time: {}", dt);
            last = now;

            m_Window->PollEvents();
            Input::InputState inputState = m_Window->ReadInput();

            for (auto& layer : m_Layers)
                layer->OnUpdate(dt);

            // TODO: Should be a dedicated entity.OnUpdate(dt) call instead of directly manipulating the model matrix here
            glm::vec3 rotationAxis = glm::normalize(glm::vec3(0.5f, 1.0f, 0.0f));
            glm::mat4 modelMatrix = glm::rotate(glm::mat4(1.0f), totalTime * cubeRotationSpeed, rotationAxis);
            LOG_TRACE("Momo", "Model matrix: {}", glm::to_string(modelMatrix));

            m_Camera->OnUpdate(dt, inputState);

            m_Renderer.RenderFrame(*m_Mesh, m_Camera->GetViewMatrix(), modelMatrix);

            // crude temporary limiter so the console doesn't spam
            // std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

    void Application::Shutdown()
    {
        if (m_IsShutdown)
        {
            LOG_WARN("Momo", "Shutdown() called multiple times. Ignoring...");
            return;
        }

        LOG_INFO("Momo", "Shutting down...");

        // Shutdown renderer first (destroys Vulkan instance before GLFW terminates)
        m_Renderer.Shutdown();

        // Detach all layers before they are destroyed
        for (auto& layer : m_Layers)
        {
            layer->OnDetach();
        }

        m_IsShutdown = true;
        LOG_INFO("Momo", "Shutdown complete.");
    }

    std::vector<Renderer::VulkanMeshData> Application::LoadMesh(const std::filesystem::path &path)
    {
        if (!m_ModelLoader)
        {
            LOG_ERROR("Momo", "Model loader not initialized.");
            throw std::runtime_error("Model loader not initialized.");
        }

        auto sceneMeshData = m_ModelLoader->LoadModel(path);
        if (!sceneMeshData)
        {
            LOG_ERROR("Momo", "Failed to load model from path: {}", path.string());
            throw std::runtime_error("Failed to load model from path: " + path.string());
        }

        try {
            std::vector<Renderer::VulkanMeshData> meshDataVec;
            for (const auto& meshData : *sceneMeshData)
            {
                // TODO: Consider caching vertex and index buffers to avoid recreating them for the same mesh.
                meshDataVec.push_back(Renderer::VulkanMeshData {
                    .m_VertexBuffer = m_Renderer.CreateVertexBuffer(meshData.vertices),
                    .m_IndexBuffer = m_Renderer.CreateIndexBuffer(meshData.indices),
                    .localTransform = meshData.localTransform,
                    .baseColorFactor = meshData.baseColorFactor,
                });
            }
            return meshDataVec;
        } catch (const std::exception& e) {
            LOG_ERROR("Momo", "Exception occurred while loading mesh: {}", e.what());
            throw;
        }
    }
} // namespace Momo