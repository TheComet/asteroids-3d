#include "Asteroids/UserRegistry/ClientUserRegistry.hpp"

#include <Urho3D/Network/NetworkEvents.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
ClientUserRegistry::ClientUserRegistry(Context* context) :
    Object(context)
{
    SubscribeToEvent(E_SERVERCONNECTED, URHO3D_HANDLER(ClientUserRegistry, HandleServerConnected));
    SubscribeToEvent(E_SERVERDISCONNECTED, URHO3D_HANDLER(ClientUserRegistry, HandleServerDisconnected));
}

// ----------------------------------------------------------------------------
void ClientUserRegistry::HandleServerConnected(StringHash eventType, VariantMap& eventData)
{
}

// ----------------------------------------------------------------------------
void ClientUserRegistry::HandleServerDisconnected(StringHash eventType, VariantMap& eventData)
{
}

}
