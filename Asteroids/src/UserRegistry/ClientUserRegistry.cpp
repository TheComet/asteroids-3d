#include "Asteroids/Network/Protocol.hpp"
#include "Asteroids/UserRegistry/ClientUserRegistry.hpp"
#include "Asteroids/UserRegistry/UserRegistry.hpp"
#include "Asteroids/UserRegistry/UserRegistryEvents.hpp"

#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Network/Network.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
ClientUserRegistry::ClientUserRegistry(Context* context) :
    Object(context)
{
    SubscribeToEvent(E_USERJOINED, URHO3D_HANDLER(ClientUserRegistry, HandleUserJoined));
    SubscribeToEvent(E_USERLEFT, URHO3D_HANDLER(ClientUserRegistry, HandleUserLeft));
}

// ----------------------------------------------------------------------------
void ClientUserRegistry::TryRegister(const String& name, const String& ipAddress, unsigned short port, Scene* scene)
{
    SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(ClientUserRegistry, HandleNetworkMessage));
    SubscribeToEvent(E_CONNECTFAILED, URHO3D_HANDLER(ClientUserRegistry, HandleConnectFailed));
    SubscribeToEvent(E_SERVERDISCONNECTED, URHO3D_HANDLER(ClientUserRegistry, HandleServerDisconnected));

    VariantMap identity;
    identity["Username"] = name;
    if (GetSubsystem<Network>()->Connect(ipAddress, port, scene, identity) == false)
        NotifyRegisterFailed("Failed to initiate connection");
}

// ----------------------------------------------------------------------------
void ClientUserRegistry::NotifyRegisterFailed(const String& reason)
{
    UnsubscribeFromEvent(E_NETWORKMESSAGE);
    UnsubscribeFromEvent(E_CONNECTFAILED);
    UnsubscribeFromEvent(E_SERVERDISCONNECTED);

    VariantMap& eventData = GetEventDataMap();
    eventData[RegisterFailed::P_REASON] = reason;
    SendEvent(E_REGISTERFAILED, eventData);
}

// ----------------------------------------------------------------------------
void ClientUserRegistry::HandleNetworkMessage(StringHash eventType, VariantMap& eventData)
{
    using namespace NetworkMessage;

    if (eventData[P_MESSAGEID].GetInt() != MSG_REGISTER_FAILED)
        return;

    MemoryBuffer buffer(eventData[P_DATA].GetBuffer());
    MsgRegisterFailed reason = static_cast<MsgRegisterFailed>(buffer.ReadUByte());
    String reasonStr = "Unknown error";
    switch (reason)
    {
        case USERNAME_TOO_LONG :
            reasonStr = "Username exceeds " + String(static_cast<int>(buffer.ReadUByte())) + " characters";
            break;

        case USERNAME_EMPTY :
            reasonStr = "Username is empty";
            break;

        case USERNAME_ALREADY_TAKEN :
            reasonStr = "Username already taken";
            break;

        case USERNAME_BANNED :
            reasonStr = "Username banned";
            break;
    }

    NotifyRegisterFailed(reasonStr);
}

// ----------------------------------------------------------------------------
void ClientUserRegistry::HandleConnectFailed(StringHash eventType, VariantMap& eventData)
{
    NotifyRegisterFailed("Failed to connect to server");
}

// ----------------------------------------------------------------------------
void ClientUserRegistry::HandleServerDisconnected(StringHash eventType, VariantMap& eventData)
{
    NotifyRegisterFailed("Disconnected by server");
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

    uint32_t guid = eventData[P_GUID].GetUInt();
    String username = eventData[P_USERNAME].GetString();
    reg->AddUser(username, guid);
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

    uint32_t guid = eventData[P_GUID].GetUInt();
    reg->RemoveUser(guid);
}

}
