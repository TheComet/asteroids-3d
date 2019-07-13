#pragma once

#include "Asteroids/Config.hpp"
#include <Urho3D/Scene/Component.h>

namespace Urho3D
{
    class XMLFile;
}

namespace Asteroids {

class ASTEROIDS_PUBLIC_API OrbitingCameraController : public Urho3D::Component
{
    URHO3D_OBJECT(OrbitingCameraController, Urho3D::Component)

public:
    OrbitingCameraController(Urho3D::Context* context);
    void SetTrackNode(Urho3D::Node* nodeToTrack);

    static void RegisterObject(Urho3D::Context* context);
    void SetConfig(Urho3D::XMLFile* config);
    Urho3D::ResourceRef GetConfigAttr() const;
    void SetConfigAttr(const Urho3D::ResourceRef& value);

private:
    void ParseCamConfig();
    void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::SharedPtr<Urho3D::XMLFile> configFile_;
    Urho3D::WeakPtr<Urho3D::Node> trackNode_;
    struct CamConfig
    {
        float distance_;
        float lookAhead_;
        float smooth_;
    } camConfig_;
};

}
