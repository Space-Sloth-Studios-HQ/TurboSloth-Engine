#include <Engine/Core/Application.h>
#include <Engine/Logging/Logger.h>

class AppLayer : public Engine::Layer
{
public:
    void OnAttach() override
    {
        LOG_INFO("AppLayer", "Application layer attached.");
    }

    void OnUpdate(float dt) override
    {
        // Update logic here
    }
};

int main()
{
    Engine::ApplicationSpecification appSpec;
    appSpec.Name = "KHClone";
    appSpec.WindowSpec.Width = 1920;
    appSpec.WindowSpec.Height = 1080;

    Engine::Application app(appSpec);
    app.PushLayer<AppLayer>();
    app.Run();

    // Explicitly shutdown before destruction
    // This ensures Vulkan instance is destroyed before GLFW terminates
    app.Shutdown();

    return 0;
}