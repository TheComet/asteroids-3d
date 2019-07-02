#pragma once

#include <Urho3D/Engine/Application.h>

namespace Urho3D {
    class DebugHud;
    class Scene;
}

namespace Asteroids {

class ServerApplication : public Urho3D::Application
{
public:
    ServerApplication(Urho3D::Context* context);

    virtual void Setup() override;
    virtual void Start() override;
    virtual void Stop() override;

private:
    void SubscribeToEvents();
    void LoadScene();
    void HandleUserJoined(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::SharedPtr<Urho3D::Scene> scene_;
};

}
