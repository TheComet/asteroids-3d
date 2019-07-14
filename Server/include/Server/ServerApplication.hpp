#pragma once

#include <Urho3D/Engine/Application.h>
#include "Asteroids/UserRegistry/User.hpp"

namespace Urho3D {
    class DebugHud;
    class Scene;
    class Node;
    class XMLFile;
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
    void HandleUserLeft(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandlePlayerCreate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandlePlayerDestroy(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::SharedPtr<Urho3D::Scene> scene_;
    Urho3D::SharedPtr<Urho3D::XMLFile> planetXML_;
    Urho3D::Node* planet_;
    Urho3D::HashMap<User::GUID, Urho3D::Node*> shipNodes_;
};

}
