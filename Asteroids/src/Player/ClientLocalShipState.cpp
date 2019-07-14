#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Player/ActionState.hpp"
#include "Asteroids/Player/ClientLocalShipState.hpp"
#include "Asteroids/Player/ShipController.hpp"
#include "Asteroids/Network/Protocol.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
ClientLocalShipState::ClientLocalShipState(Context* context) :
    Component(context),
    timeStep_(0),
    lastTimeStep_(0)
{
    SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(ClientLocalShipState, HandleNetworkMessage));
    SubscribeToEvent(E_NETWORKUPDATE, URHO3D_HANDLER(ClientLocalShipState, HandleNetworkUpdate));
}

// ----------------------------------------------------------------------------
void ClientLocalShipState::RegisterObject(Urho3D::Context* context)
{
    context->RegisterFactory<ClientLocalShipState>(ASTEROIDS_CATEGORY);
}

// ----------------------------------------------------------------------------
void ClientLocalShipState::SetUser(User* user)
{
    user_ = user;
}

// ----------------------------------------------------------------------------
void ClientLocalShipState::HandleNetworkMessage(StringHash eventType, VariantMap& eventData)
{
    using namespace NetworkMessage;

    if (eventData[P_MESSAGEID].GetInt() != MSG_SERVER_SHIP_STATE)
        return;
    if (user_.Expired())
        return;

    MemoryBuffer buffer(eventData[P_DATA].GetBuffer());
    User::GUID guid = buffer.ReadUShort();
    if (guid != user_->GetGUID())
        return;

    // Only update action state if timestamp is newer than the last one we
    // received
    uint8_t timeStep = buffer.ReadUByte();
    if ((signed char)(timeStep - lastTimeStep_) <= 0)
        return;

    lastTimeStep_ = timeStep;

    Quaternion pivotRotation = buffer.ReadPackedQuaternion();
    float planetHeight = buffer.ReadFloat();
    float shipAngle = buffer.ReadFloat();

    // TODO prediction. For now just take server state directly
    Node* pivot = node_->GetParent();
    pivot->SetRotation(pivotRotation);
    node_->GetComponent<ShipController>()->SetAngle(shipAngle);
}

// ----------------------------------------------------------------------------
void ClientLocalShipState::HandleNetworkUpdate(StringHash eventType, VariantMap& eventData)
{
    ActionState* state = GetComponent<ActionState>();
    Network* network = GetSubsystem<Network>();
    Connection* connection = network->GetServerConnection();

    assert(state);

    if (connection == nullptr)  // no server to send to
        return;
    if (user_.Expired())
        return;

    msg_.Clear();
    msg_.WriteUShort(user_->GetGUID());
    msg_.WriteUByte(timeStep_++);
    msg_.WriteUShort(state->GetState());
    connection->SendMessage(MSG_CLIENT_SHIP_STATE, false, false, msg_);
}

}
