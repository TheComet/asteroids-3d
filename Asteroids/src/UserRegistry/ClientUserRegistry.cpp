#include "Asteroids/UserRegistry/ClientUserRegistry.hpp"
#include "Asteroids/UserRegistry/UserRegistry.hpp"
#include "Asteroids/UserRegistry/UserRegistryEvents.hpp"

#include <Urho3D/IO/Log.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
ClientUserRegistry::ClientUserRegistry(Context* context) :
    Object(context)
{
    SubscribeToEvent(E_USERJOINED, URHO3D_HANDLER(ClientUserRegistry, HandleUserJoined));
    SubscribeToEvent(E_USERLEFT, URHO3D_HANDLER(ClientUserRegistry, HandleUserLeft));
    SubscribeToEvent(E_USERLIST, URHO3D_HANDLER(ClientUserRegistry, HandleUserList));
}

// ----------------------------------------------------------------------------
void ClientUserRegistry::HandleUserJoined(StringHash eventType, VariantMap& eventData)
{
    using namespace UserJoined;

    UserRegistry* reg = GetSubsystem<UserRegistry>();
    if (reg == nullptr)
    {
        URHO3D_LOGERROR("UserRegistry subsystem is not registered");
        return;
    }

    String username = eventData[P_USERNAME].GetString();
    if (reg->AddUser(username, nullptr) == false)
    {
        URHO3D_LOGERRORF("Failed to add received username \"%s\". This should not happen!", username.CString());
    }


    URHO3D_LOGDEBUG("Current userlist:");
    for (const auto& user : reg->GetUsers())
        URHO3D_LOGDEBUGF(" -- %s", user.second_.username_.CString());
}

// ----------------------------------------------------------------------------
void ClientUserRegistry::HandleUserLeft(StringHash eventType, VariantMap& eventData)
{
    using namespace UserLeft;

    UserRegistry* reg = GetSubsystem<UserRegistry>();
    if (reg == nullptr)
    {
        URHO3D_LOGERROR("UserRegistry subsystem is not registered");
        return;
    }

    String username = eventData[P_USERNAME].GetString();
    if (reg->RemoveUser(username) == false)
    {
        URHO3D_LOGERRORF("Failed to add received username \"%s\". This should not happen!", username.CString());
    }

    URHO3D_LOGDEBUG("Current userlist:");
    for (const auto& user : reg->GetUsers())
        URHO3D_LOGDEBUGF(" -- %s", user.second_.username_.CString());
}

// ----------------------------------------------------------------------------
void ClientUserRegistry::HandleUserList(StringHash eventType, VariantMap& eventData)
{
    using namespace UserList;

    UserRegistry* reg = GetSubsystem<UserRegistry>();
    if (reg == nullptr)
    {
        URHO3D_LOGERROR("UserRegistry subsystem is not registered");
        return;
    }

    const StringVector& users = eventData[P_USERS].GetStringVector();
    reg->ClearAll();
    for (const auto& user : users)
        if (reg->AddUser(user, nullptr) == false)
        {
            URHO3D_LOGERRORF("Failed to add received username \"%s\". This should not happen!", user.CString());
        }


    URHO3D_LOGDEBUG("Current userlist:");
    for (const auto& user : reg->GetUsers())
        URHO3D_LOGDEBUGF(" -- %s", user.second_.username_.CString());
}

}
