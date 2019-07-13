#include "Server/ServerApplication.hpp"
#include "Server/SignalHandler.hpp"
#include "Asteroids/Globals.hpp"
#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Util/DebugTextScroll.hpp"
#include "Asteroids/UserRegistry/ServerUserRegistry.hpp"
#include "Asteroids/UserRegistry/UserRegistry.hpp"
#include "Asteroids/UserRegistry/UserRegistryEvents.hpp"

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/Scene.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
ServerApplication::ServerApplication(Context* context) :
    Application(context)
{
}

// ----------------------------------------------------------------------------
void ServerApplication::Setup()
{
    engineParameters_[EP_LOG_NAME] = "asteroids-server.log";
    engineParameters_[EP_HEADLESS] = true;
}

// ----------------------------------------------------------------------------
void ServerApplication::Start()
{
    RegisterObjectFactories(context_);
    RegisterRemoteNetworkEvents(context_);

    context_->RegisterSubsystem<SignalHandler>();
    context_->RegisterSubsystem<UserRegistry>();
    context_->RegisterSubsystem<ServerUserRegistry>();

#if defined(DEBUG)
    GetSubsystem<Log>()->SetLevel(LOG_DEBUG);
#endif

    // Configure resource cache to auto-reload resources when they change on
    // the filesystem
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    cache->SetAutoReloadResources(true);

    SubscribeToEvents();
    LoadScene();

    // Start server
    GetSubsystem<Network>()->StartServer(DEFAULT_PORT);
}

// ----------------------------------------------------------------------------
void ServerApplication::Stop()
{
    Network* network = GetSubsystem<Network>();
    network->StopServer();
}

// ----------------------------------------------------------------------------
void ServerApplication::LoadScene()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>();
    scene_->CreateComponent<PhysicsWorld>();

    Node* planetNode = scene_->CreateChild("Planet");
    planetNode->LoadXML(cache->GetResource<XMLFile>("Prefabs/TestPlanet.xml")->GetRoot());
}

// ----------------------------------------------------------------------------
void ServerApplication::SubscribeToEvents()
{
    SubscribeToEvent(E_USERJOINED, URHO3D_HANDLER(ServerApplication, HandleUserJoined));
}

// ----------------------------------------------------------------------------
void ServerApplication::HandleUserJoined(StringHash eventType, VariantMap& eventData)
{
    using namespace UserJoined;

    uint32_t guid = eventData[P_GUID].GetUInt();
    const User* user = GetSubsystem<UserRegistry>()->GetUser(guid);

    assert(user->GetConnection() != nullptr);
    user->GetConnection()->SetScene(scene_);
}

// ----------------------------------------------------------------------------
void ServerApplication::HandleUserLeft(StringHash eventType, VariantMap& eventData)
{
    using namespace UserLeft;
}

}
