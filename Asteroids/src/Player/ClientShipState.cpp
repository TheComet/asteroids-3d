#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Player/ActionState.hpp"
#include "Asteroids/Player/ClientShipState.hpp"
#include "Asteroids/Network/Protocol.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Scene/Node.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
ClientShipState::ClientShipState(Context* context) :
    Component(context),
    timeStep_(0)
{
    SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(ClientShipState, HandleNetworkMessage));
    SubscribeToEvent(E_NETWORKUPDATE, URHO3D_HANDLER(ClientShipState, HandleNetworkUpdate));
}

// ----------------------------------------------------------------------------
void ClientShipState::RegisterObject(Urho3D::Context* context)
{
    context->RegisterFactory<ClientShipState>(ASTEROIDS_CATEGORY);
}

// ----------------------------------------------------------------------------
void ClientShipState::HandleNetworkMessage(StringHash eventType, VariantMap& eventData)
{
    using namespace NetworkMessage;

    if (eventData[P_MESSAGEID].GetInt() != MSG_SERVER_SHIP_STATE)
        return;

    MemoryBuffer buffer(eventData[P_DATA].GetBuffer());
    uint8_t timeStep = buffer.ReadUByte();

    // Only update action state if timestamp is newer than the last one we
    // received
    if ((signed char)(timeStep - lastTimeStep_) <= 0)
        return;

    uint16_t actionState = buffer.ReadUShort();
    Quaternion pivotRotation = buffer.ReadPackedQuaternion();

    // TODO prediction
    Node* pivot = node_->GetParent();
    pivot->SetRotation(pivotRotation);
}

// ----------------------------------------------------------------------------
void ClientShipState::HandleNetworkUpdate(StringHash eventType, VariantMap& eventData)
{
    ActionState* state = GetComponent<ActionState>();
    Network* network = GetSubsystem<Network>();
    Connection* connection = network->GetServerConnection();

    assert(state);
    assert(connection);

    msg_.Clear();
    msg_.WriteUByte(timeStep_);
    msg_.WriteUShort(state->GetState());
    connection->SendMessage(MSG_CLIENT_SHIP_STATE, false, false, msg_);
}

}
