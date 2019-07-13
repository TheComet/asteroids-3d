#pragma once

#include "Asteroids/Config.hpp"
#include <Urho3D/Core/Object.h>

namespace Urho3D {
    class Scene;
}

namespace Asteroids {

class ASTEROIDS_PUBLIC_API ClientUserRegistry : public Urho3D::Object
{
    URHO3D_OBJECT(ClientUserRegistry, Urho3D::Object)

public:
    ClientUserRegistry(Urho3D::Context* context);

    void TryRegister(const Urho3D::String& name, const Urho3D::String& ipAddress, unsigned short port, Urho3D::Scene* scene);

private:
    void HandleNetworkMessage(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleConnectFailed(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleServerDisconnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    void NotifyRegisterFailed(const Urho3D::String& reason);
    void HandleUserJoined(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleUserLeft(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
};

}
