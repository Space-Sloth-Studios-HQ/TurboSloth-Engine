#include <iostream>
#include <Engine/Core/Application.h>

class AppLayer : public Engine::Layer 
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
    Engine::ApplicationSpecification appSpec;
    appSpec.Name = "KHClone";
    appSpec.WindowSpec.Title = "KHClone Window";
    appSpec.WindowSpec.Width = 1920;
    appSpec.WindowSpec.Height = 1080;

    Engine::Application app(appSpec);
    app.PushLayer<AppLayer>();
    app.Run();
}