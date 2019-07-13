#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Player/ActionState.hpp"
#include "Asteroids/Player/ServerShipState.hpp"
#include "Asteroids/Network/Protocol.hpp"
#include "Asteroids/UserRegistry/User.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Network/Connection.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/IO/Log.h>

#include "Asteroids/Player/ShipController.hpp"

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
ServerShipState::ServerShipState(Context* context) :
    Component(context),
    lastTimeStep_(0)
{
    SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(ServerShipState, HandleNetworkMessage));
    SubscribeToEvent(E_NETWORKUPDATE, URHO3D_HANDLER(ServerShipState, HandleNetworkUpdate));
}

// ----------------------------------------------------------------------------
void ServerShipState::RegisterObject(Context* context)
{
    context->RegisterFactory<ServerShipState>(ASTEROIDS_CATEGORY);
}

// ----------------------------------------------------------------------------
void ServerShipState::SetUser(User* user)
{
    user_ = user;
}

// ----------------------------------------------------------------------------
void ServerShipState::HandleNetworkMessage(StringHash eventType, VariantMap& eventData)
{
    using namespace NetworkMessage;

    if (eventData[P_MESSAGEID].GetInt() != MSG_CLIENT_SHIP_STATE)
        return;
    if (user_.Expired())
        return;

    MemoryBuffer buffer(eventData[P_DATA].GetBuffer());
    User::GUID userGUID = buffer.ReadUShort();

    if (userGUID != user_->GetGUID())
        return;

    // Only update action state if timestamp is newer than the last one we
    // received
    uint8_t timeStep = buffer.ReadUByte();
    if ((signed char)(timeStep - lastTimeStep_) <= 0)
        return;

    lastTimeStep_ = timeStep;

    ActionState* actionState = GetComponent<ActionState>();
    ActionState::Data state = buffer.ReadUShort();
    actionState->SetState(state);
    assert(GetComponent<ShipController>());
}

// ----------------------------------------------------------------------------
void ServerShipState::HandleNetworkUpdate(StringHash eventType, VariantMap& eventData)
{
    Node* pivot = node_->GetParent();

    if (user_.Expired())
        return;

    msg_.Clear();
    msg_.WriteUShort(user_->GetGUID());

    msg_.WriteUByte(lastTimeStep_);
    msg_.WritePackedQuaternion(pivot->GetRotation());
    msg_.WriteFloat(node_->GetRotation().EulerAngles().y_);
    user_->GetConnection()->SendMessage(MSG_SERVER_SHIP_STATE, false, false, msg_);
}

}
