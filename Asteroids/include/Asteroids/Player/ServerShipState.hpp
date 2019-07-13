#pragma once

#include "Asteroids/Config.hpp"
#include <Urho3D/Scene/Component.h>
#include <Urho3D/IO/VectorBuffer.h>

namespace Asteroids {

class ServerShipState : public Urho3D::Component
{
    URHO3D_OBJECT(ServerShipState, Urho3D::Component)

public:
    ServerShipState(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

private:
    void HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleNetworkUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    uint8_t lastTimeStep_;
    Urho3D::VectorBuffer msg_;
};

}
