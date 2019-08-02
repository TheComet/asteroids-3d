#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Menu/ConnectPrompt.hpp"
#include "Asteroids/Menu/HostServerPrompt.hpp"
#include "Asteroids/Menu/MainMenu.hpp"
#include "Asteroids/Objects/Asteroid.hpp"
#include "Asteroids/Objects/MineController.hpp"
#include "Asteroids/Objects/PhaserController.hpp"
#include "Asteroids/Player/ActionState.hpp"
#include "Asteroids/Player/ClientLocalShipState.hpp"
#include "Asteroids/Player/ClientRemoteShipState.hpp"
#include "Asteroids/Player/DeviceInputMapper.hpp"
#include "Asteroids/Player/OrbitingCameraController.hpp"
#include "Asteroids/Player/PlayerEvents.hpp"
#include "Asteroids/Player/ServerShipState.hpp"
#include "Asteroids/Player/ShipController.hpp"
#include "Asteroids/Player/WeaponSpawner.hpp"
#include "Asteroids/UserRegistry/UserRegistryEvents.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

namespace Asteroids {

const char* ASTEROIDS_CATEGORY = "Asteroids";

// ----------------------------------------------------------------------------
void RegisterObjectFactories(Context* context)
{
    // TODO convert these to static methods
    context->RegisterFactory<Asteroid>(ASTEROIDS_CATEGORY);

    ActionState::RegisterObject(context);
    ClientLocalShipState::RegisterObject(context);
    ClientRemoteShipState::RegisterObject(context);
    ConnectPrompt::RegisterObject(context);
    DeviceInputMapper::RegisterObject(context);
    HostServerPrompt::RegisterObject(context);
    MainMenu::RegisterObject(context);
    MineController::RegisterObject(context);
    OrbitingCameraController::RegisterObject(context);
    PhaserController::RegisterObject(context);
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

// ----------------------------------------------------------------------------
RenderPath* LoadRenderPath(Context* context)
{
    Graphics* graphics = context->GetSubsystem<Graphics>();
    ResourceCache* cache = context->GetSubsystem<ResourceCache>();

    // Load render path configurations
    SharedPtr<XMLFile> baseRenderPath;
    if (graphics->GetReadableDepthSupport())
        baseRenderPath = cache->GetResource<XMLFile>("RenderPaths/AsteroidsDeferredHWDepth.xml");
    else
        baseRenderPath = cache->GetResource<XMLFile>("RenderPaths/AsteroidsDeferred.xml");
    SharedPtr<XMLFile> emissiveGlow(cache->GetResource<XMLFile>("PostProcess/EmissiveGlow.xml"));
    SharedPtr<XMLFile> fxaa3(cache->GetResource<XMLFile>("PostProcess/FXAA3.xml"));

    // Create renderpath and return
    RenderPath* renderPath = new RenderPath;
    renderPath->Append(baseRenderPath);
    renderPath->Append(emissiveGlow);
    renderPath->Append(fxaa3);
    return renderPath;
}

}
