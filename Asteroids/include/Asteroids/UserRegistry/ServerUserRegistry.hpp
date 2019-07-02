#pragma once

#include "Asteroids/Config.hpp"
#include <Urho3D/Core/Object.h>

namespace Asteroids {

class ASTEROIDS_PUBLIC_API ServerUserRegistry : public Urho3D::Object
{
    URHO3D_OBJECT(ServerUserRegistry, Urho3D::Object)

public:
    ServerUserRegistry(Urho3D::Context* context);

private:
    void HandleClientIdentity(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleClientDisconnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};

}
