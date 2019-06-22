#pragma once

#include "Asteroids/Config.hpp"
#include <Urho3D/Scene/Component.h>

namespace Asteroids {

class ASTEROIDS_PUBLIC_API OrbitingCameraController : public Urho3D::Component
{
    URHO3D_OBJECT(OrbitingCameraController, Urho3D::Component)

public:
    OrbitingCameraController(Urho3D::Context* context);

    void SetTrackNode(Urho3D::Node* nodeToTrack);
    void SetDistance(float distance);

private:
    void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::WeakPtr<Urho3D::Node> trackNode_;
    float distance_;
};

}
