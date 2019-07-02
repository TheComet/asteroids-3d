#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Network/Protocol.hpp"
#include "Asteroids/Network/NetworkShipState.hpp"
#include "Asteroids/Network/NetworkEvents.hpp"
#include "Asteroids/Player/ActionState.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Scene/Node.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
NetworkShipState::NetworkShipState(Context* context) :
    Component(context)
{
}

// ----------------------------------------------------------------------------
void NetworkShipState::RegisterObject(Context* context)
{
    context->RegisterFactory<NetworkShipState>(ASTEROIDS_CATEGORY);
}

// ----------------------------------------------------------------------------
void NetworkShipState::HandleNetworkMessage(StringHash eventType, VariantMap& eventData)
{
    using namespace NetworkMessage;

    MemoryBuffer buffer(eventData[P_DATA].GetBuffer());
    Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());

    switch (eventData[P_MESSAGEID].GetInt())
    {
        case MSG_CLIENT_SHIP_STATE : {
            // Bitfield of current input state
            uint8_t timestamp = connection->GetTimeStamp();
        }
    }
}

// ----------------------------------------------------------------------------
void NetworkShipState::HandleNetworkUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace NetworkUpdate;

    Network* network = GetSubsystem<Network>();
    Connection* connection = network->GetServerConnection();

    if (connection)  // We are client
    {
        ActionState* state = GetComponent<ActionState>();
        if (state == nullptr)
        {
            URHO3D_LOGERROR("NetworkShipState requires the ActionState component to be attached. Don't know what state to send to server!");
            return;
        }

        msg_.Clear();
        state->SaveStateNet(msg_);
    }
    else  // We are server
    {
        Node* pivot = node_->GetParent();

        msg_.Clear();
        msg_.WritePackedQuaternion(pivot->GetRotation());
    }
}

}
