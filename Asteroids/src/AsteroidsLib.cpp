#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Menu/ConnectPrompt.hpp"
#include "Asteroids/Menu/MainMenu.hpp"
#include "Asteroids/Player/ActionState.hpp"
#include "Asteroids/Player/DeviceInputMapper.hpp"
#include "Asteroids/Player/OrbitingCameraController.hpp"
#include "Asteroids/Player/ShipController.hpp"
#include "Asteroids/Objects/Asteroid.hpp"
#include "Asteroids/Objects/Bullet.hpp"
#include "Asteroids/Network/NetworkShipState.hpp"
#include "Asteroids/UserRegistry/UserRegistryEvents.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>

using namespace Urho3D;

namespace Asteroids {

const char* ASTEROIDS_CATEGORY = "Asteroids";

// ----------------------------------------------------------------------------
void RegisterObjectFactories(Context* context)
{
    context->RegisterFactory<Asteroid>(ASTEROIDS_CATEGORY);
    context->RegisterFactory<Bullet>(ASTEROIDS_CATEGORY);
    context->RegisterFactory<OrbitingCameraController>(ASTEROIDS_CATEGORY);

    ActionState::RegisterObject(context);
    ConnectPrompt::RegisterObject(context);
    DeviceInputMapper::RegisterObject(context);
    MainMenu::RegisterObject(context);
    NetworkShipState::RegisterObject(context);
    ShipController::RegisterObject(context);
}

// ----------------------------------------------------------------------------
void RegisterRemoteNetworkEvents(Context* context)
{
    // Register events that can be sent over the network
    Network* network = context->GetSubsystem<Network>();
    network->RegisterRemoteEvent(E_REGISTERSUCCEEDED);
    network->RegisterRemoteEvent(E_USERJOINED);
    network->RegisterRemoteEvent(E_USERLEFT);
    network->RegisterRemoteEvent(E_USERLIST);
}

}
