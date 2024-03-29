#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Player/ActionState.hpp"
#include "Asteroids/Player/ClientRemoteShipState.hpp"
#include "Asteroids/Player/ShipController.hpp"
#include "Asteroids/Network/Protocol.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/XMLFile.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
ClientRemoteShipState::ClientRemoteShipState(Context* context) :
    Component(context),
    timeStep_(0),
    lastTimeStep_(0)
{
    SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(ClientRemoteShipState, HandleNetworkMessage));
}

// ----------------------------------------------------------------------------
void ClientRemoteShipState::RegisterObject(Urho3D::Context* context)
{
    context->RegisterFactory<ClientRemoteShipState>(ASTEROIDS_CATEGORY);
}

// ----------------------------------------------------------------------------
void ClientRemoteShipState::SetUser(User* user)
{
    user_ = user;
}

// ----------------------------------------------------------------------------
void ClientRemoteShipState::HandleNetworkMessage(StringHash eventType, VariantMap& eventData)
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
    node_->SetPosition(Vector3(0, planetHeight, 0));
    node_->SetRotation(Quaternion(0, shipAngle, 0));
}

}
