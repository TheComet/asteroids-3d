#include "Asteroids/UserRegistry/ServerUserRegistry.hpp"
#include "Asteroids/UserRegistry/UserRegistry.hpp"
#include "Asteroids/UserRegistry/UserRegistryEvents.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>
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

    // Username might be empty (or not exist)
    if (username.Empty())
    {
        URHO3D_LOGERROR("Empty username, rejecting");
        eventData[P_ALLOW] = false;

        connection->SendRemoteEvent(E_INVALIDUSERNAME, true);

        return;
    }

    // Username might be too long
    if (username.Length() > 50)
    {
        URHO3D_LOGERROR("Username exceeds maximum length, rejecting");
        eventData[P_ALLOW] = false;

        VariantMap data;
        data[UsernameTooLong::P_MAXLENGTH] = 50;
        connection->SendRemoteEvent(E_USERNAMETOOLONG, true, data);

        return;
    }

    // User registry subsystem might not exist
    UserRegistry* reg = GetSubsystem<UserRegistry>();
    if (reg == nullptr)
    {
        URHO3D_LOGERRORF("Can't accept client with username \"%s\", UserRegistry subsystem doesn't exist", username.CString());
        eventData[P_ALLOW] = false;
        return;
    }

    // Try to add the user. If the username already exists, reject
    if (reg->AddUser(username, connection) == false)
    {
        URHO3D_LOGERRORF("Username \"%s\" already exists, rejecting", username.CString());
        eventData[P_ALLOW] = false;

        connection->SendRemoteEvent(E_USERNAMEALREADYEXISTS, true);

        return;
    }

    // Success, let everyone know a new user joined
    VariantMap data;
    data[UserJoined::P_USERNAME] = username;
    GetSubsystem<Network>()->BroadcastRemoteEvent(E_USERJOINED, true, data);
    SendEvent(E_USERJOINED, data);

    // Create vector of strings of all joined users
    StringVector users;
    users.Reserve(reg->GetUsers().Size());
    for (const auto& user : reg->GetUsers())
        users.Push(user.second_.username_);

    // Send list to client that just connected
    data.Clear();
    data[UserList::P_USERS] = users;
    connection->SendRemoteEvent(E_USERLIST, true, data);


    URHO3D_LOGDEBUG("Current userlist:");
    for (const auto& user : reg->GetUsers())
        URHO3D_LOGDEBUGF(" -- %s", user.second_.username_.CString());
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

    // Only send the event if the user exists before removal
    if (reg->RemoveUser(connection))
    {
        VariantMap& data = GetEventDataMap();
        data[UserLeft::P_USERNAME] = connection->GetIdentity()["Username"].GetString();
        GetSubsystem<Network>()->BroadcastRemoteEvent(E_USERLEFT, true, data);
    }


    URHO3D_LOGDEBUG("Current userlist:");
    for (const auto& user : reg->GetUsers())
        URHO3D_LOGDEBUGF(" -- %s", user.second_.username_.CString());
}

}
