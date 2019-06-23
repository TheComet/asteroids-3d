#pragma once

#include <Urho3D/Engine/Application.h>

namespace Urho3D {
    class DebugHud;
    class Scene;
}

namespace Asteroids {

class ClientApplication : public Urho3D::Application
{
public:
    ClientApplication(Urho3D::Context* context);

    virtual void Setup() override;
    virtual void Start() override;
    virtual void Stop() override;

private:
    void SubscribeToEvents();
    void CreateDebugHud();

    void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandlePostRenderUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleServerConnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleServerDisconnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleConnectFailed(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::SharedPtr<Urho3D::Scene> scene_;
    Urho3D::SharedPtr<Urho3D::DebugHud> debugHud_;
    bool drawPhyGeometry_;
};

}
