#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Player/ActionState.hpp"
#include "Asteroids/Player/ServerShipState.hpp"
#include "Asteroids/Network/Protocol.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Scene/Node.h>

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
void ServerShipState::HandleNetworkMessage(StringHash eventType, VariantMap& eventData)
{
    using namespace NetworkMessage;

    if (eventData[P_MESSAGEID].GetInt() != MSG_CLIENT_SHIP_STATE)
        return;

    ActionState* actionState = GetComponent<ActionState>();

    MemoryBuffer buffer(eventData[P_DATA].GetBuffer());
    uint8_t timeStep = buffer.ReadUByte();

    // Only update action state if timestamp is newer than the last one we
    // received
    if ((signed char)(timeStep - lastTimeStep_) <= 0)
        return;

    lastTimeStep_ = timeStep;
    actionState->SetState(buffer.ReadUShort());
}

// ----------------------------------------------------------------------------
void ServerShipState::HandleNetworkUpdate(StringHash eventType, VariantMap& eventData)
{
    Node* pivot = node_->GetParent();
    ActionState* actionState = GetComponent<ActionState>();

    msg_.Clear();
    msg_.WriteUByte(lastTimeStep_);
    msg_.WriteUShort(actionState->GetState());
    msg_.WritePackedQuaternion(pivot->GetRotation());
}

}
