#pragma once

#include "Asteroids/UserRegistry/User.hpp"

#include <Urho3D/Engine/Application.h>

namespace Urho3D {
    class DebugHud;
    class Scene;
    class Node;
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
    void ParseArgs();

    void HandleConnectPromptRequestConnect(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleMainMenuQuit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandlePlayerCreate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandlePlayerDestroy(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandlePostRenderUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleRegisterSucceeded(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::SharedPtr<Urho3D::Scene> scene_;
    Urho3D::SharedPtr<Urho3D::DebugHud> debugHud_;
    Urho3D::SharedPtr<Urho3D::Node> cameraNode_;
    bool drawPhyGeometry_;
    User::GUID myGuid_;

    struct Args
    {
        Urho3D::String username_;
    } args_;
};

}
