#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Menu/ConnectPrompt.hpp"
#include "Asteroids/Menu/HostServerPrompt.hpp"
#include "Asteroids/Menu/MainMenu.hpp"
#include "Asteroids/Player/ActionState.hpp"
#include "Asteroids/Player/ClientLocalShipState.hpp"
#include "Asteroids/Player/ClientRemoteShipState.hpp"
#include "Asteroids/Player/DeviceInputMapper.hpp"
#include "Asteroids/Player/OrbitingCameraController.hpp"
#include "Asteroids/Player/PlayerEvents.hpp"
#include "Asteroids/Player/ServerShipState.hpp"
#include "Asteroids/Player/ShipController.hpp"
#include "Asteroids/Player/WeaponSpawner.hpp"
#include "Asteroids/Objects/Asteroid.hpp"
#include "Asteroids/Objects/BulletController.hpp"
#include "Asteroids/UserRegistry/UserRegistryEvents.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Network.h>

using namespace Urho3D;

namespace Asteroids {

const char* ASTEROIDS_CATEGORY = "Asteroids";

// ----------------------------------------------------------------------------
void RegisterObjectFactories(Context* context)
{
    // TODO convert these to static methods
    context->RegisterFactory<Asteroid>(ASTEROIDS_CATEGORY);

    ActionState::RegisterObject(context);
    BulletController::RegisterObject(context);
    ClientLocalShipState::RegisterObject(context);
    ClientRemoteShipState::RegisterObject(context);
    ConnectPrompt::RegisterObject(context);
    DeviceInputMapper::RegisterObject(context);
    HostServerPrompt::RegisterObject(context);
    MainMenu::RegisterObject(context);
    OrbitingCameraController::RegisterObject(context);
    ServerShipState::RegisterObject(context);
    ShipController::RegisterObject(context);
    WeaponSpawner::RegisterObject(context);
}

// ----------------------------------------------------------------------------
void RegisterRemoteNetworkEvents(Context* context)
{
    // Register events that can be sent over the network
    Network* network = context->GetSubsystem<Network>();
    network->RegisterRemoteEvent(E_REGISTERSUCCEEDED);
    network->RegisterRemoteEvent(E_USERJOINED);
    network->RegisterRemoteEvent(E_USERLEFT);
    network->RegisterRemoteEvent(E_PLAYERCREATE);
    network->RegisterRemoteEvent(E_PLAYERDESTROY);
}

}
