#include <Momo/Core/Application.h>
#include <Momo/Logging/Logger.h>

class AppLayer : public Momo::Layer
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
    Momo::ApplicationSpecification appSpec;
    appSpec.Name = "KHClone";
    appSpec.WindowSpec.Width = 1920;
    appSpec.WindowSpec.Height = 1080;

    Momo::Application app(appSpec);
    app.PushLayer<AppLayer>();
    app.Run();

    // Explicitly shutdown before destruction
    // This ensures Vulkan instance is destroyed before GLFW terminates
    app.Shutdown();

    return 0;
}