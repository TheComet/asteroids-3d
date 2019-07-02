#pragma once

#include "Asteroids/Config.hpp"
#include <Urho3D/Scene/Component.h>
#include <Urho3D/IO/VectorBuffer.h>

namespace Asteroids {

class NetworkShipState : public Urho3D::Component
{
    URHO3D_OBJECT(NetworkShipState, Urho3D::Component)

public:
    NetworkShipState(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

private:
    void HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleNetworkUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    Urho3D::VectorBuffer msg_;
};

}
