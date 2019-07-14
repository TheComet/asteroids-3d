#pragma once

#include "Asteroids/Config.hpp"
#include "Asteroids/UserRegistry/User.hpp"
#include <Urho3D/Scene/Component.h>
#include <Urho3D/IO/VectorBuffer.h>

namespace Asteroids {

class User;

class ASTEROIDS_PUBLIC_API ClientLocalShipState : public Urho3D::Component
{
    URHO3D_OBJECT(ClientLocalShipState, Urho3D::Component)

public:
    ClientLocalShipState(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

    void SetUser(User* user);

private:
    void HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleNetworkUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::VectorBuffer msg_;
    Urho3D::WeakPtr<User> user_;
    uint8_t timeStep_;
    uint8_t lastTimeStep_;
};

}
