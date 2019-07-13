#pragma once

#include "Asteroids/Config.hpp"
#include <Urho3D/Scene/Component.h>
#include <Urho3D/IO/VectorBuffer.h>

namespace Asteroids {

class ClientShipState : public Urho3D::Component
{
    URHO3D_OBJECT(ClientShipState, Urho3D::Component)

public:
    ClientShipState(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

private:
    void HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleNetworkUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::VectorBuffer msg_;
    uint8_t timeStep_;
    uint8_t lastTimeStep_;
};

}
