#pragma once

#include "Asteroids/Config.hpp"
#include <Urho3D/Core/Object.h>

namespace Asteroids {

class ASTEROIDS_PUBLIC_API ClientUserRegistry : public Urho3D::Object
{
    URHO3D_OBJECT(ClientUserRegistry, Urho3D::Object)

public:
    ClientUserRegistry(Urho3D::Context* context);

private:
    void HandleServerConnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleServerDisconnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};

}
