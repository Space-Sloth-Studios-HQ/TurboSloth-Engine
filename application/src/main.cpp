#include <iostream>
#include <Engine/Core/Application.h>

class AppLayer : public Engine::Core::Layer 
{
public:
    void OnAttach() override 
    {
        std::cout << "[AppLayer] Attached to application.\n";
    }

    void OnUpdate(float dt) override 
    {
        // Update logic here
    }
};

int main() 
{
    Engine::Core::ApplicationSpecification appSpec;
    appSpec.Name = "KHClone";
    appSpec.WindowSpec.Width = 1920;
    appSpec.WindowSpec.Height = 1080;

    Engine::Core::Application app(appSpec);
    app.PushLayer<AppLayer>();
    app.Run();
}