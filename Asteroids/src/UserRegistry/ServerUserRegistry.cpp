#include "Asteroids/UserRegistry/ServerUserRegistry.hpp"
#include "Asteroids/UserRegistry/UserRegistry.hpp"
#include "Asteroids/UserRegistry/UserRegistryEvents.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Network/Connection.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
ServerUserRegistry::ServerUserRegistry(Context* context) :
    Object(context)
{
    SubscribeToEvent(E_CLIENTIDENTITY, URHO3D_HANDLER(ServerUserRegistry, HandleClientIdentity));
    SubscribeToEvent(E_CLIENTDISCONNECTED, URHO3D_HANDLER(ServerUserRegistry, HandleClientDisconnected));
}

// ----------------------------------------------------------------------------
void ServerUserRegistry::HandleClientIdentity(StringHash eventType, VariantMap& eventData)
{
    using namespace ClientIdentity;

    Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    String username = connection->GetIdentity()["Username"].GetString();
    if (username.Empty())
    {
        URHO3D_LOGERROR("Empty username");
        eventData[P_ALLOW] = false;
        return;
    }
    if (username.Length() > 50)
    {
        URHO3D_LOGERROR("Username exceeds maximum length");
        eventData[P_ALLOW] = false;
        return;
    }

    UserRegistry* reg = GetSubsystem<UserRegistry>();
    if (reg == nullptr)
    {
        URHO3D_LOGERRORF("Can't accept client with username \"%s\", UserRegistry subsystem doesn't exist", username.CString());
        eventData[P_ALLOW] = false;
        return;
    }

    if (reg->AddUser(username, connection) == false)
    {
        URHO3D_LOGERRORF("Username \"%s\" already exists, rejecting", username.CString());
        eventData[P_ALLOW] = false;
        return;
    }
}

// ----------------------------------------------------------------------------
void ServerUserRegistry::HandleClientDisconnected(StringHash eventType, VariantMap& eventData)
{
    using namespace ClientDisconnected;

    Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());

    UserRegistry* reg = GetSubsystem<UserRegistry>();
    if (reg == nullptr)
    {
        URHO3D_LOGERROR("HandleClientDisconnected: UserRegistry subsystem doesn't exist");
        return;
    }

    reg->RemoveUser(connection);
}

}
