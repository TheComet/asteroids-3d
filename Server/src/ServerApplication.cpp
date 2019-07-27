#include "Server/ServerApplication.hpp"
#include "Server/SignalHandler.hpp"
#include "Asteroids/Globals.hpp"
#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Player/PlayerEvents.hpp"
#include "Asteroids/Player/ServerShipState.hpp"
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
#include <Urho3D/Resource/ResourceEvents.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Core/StringUtils.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
ServerApplication::ServerApplication(Context* context) :
    Application(context),
    args_({DEFAULT_PORT})
{
}

// ----------------------------------------------------------------------------
void ServerApplication::Setup()
{
    ParseArgs();

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
    Network* network = GetSubsystem<Network>();
#if defined(DEBUG) && 0
    network->SetSimulatedLatency(200);
    network->SetSimulatedPacketLoss(0.1);
#endif
    network->StartServer(args_.port_);
}

// ----------------------------------------------------------------------------
void ServerApplication::Stop()
{
    Network* network = GetSubsystem<Network>();
    network->StopServer();
}

// ----------------------------------------------------------------------------
void ServerApplication::ParseArgs()
{
    enum Expect
    {
        EXPECT_NONE,
        EXPECT_PORT_NUMBER
    } expected = EXPECT_NONE;

    for (const auto& arg : GetArguments())
    {
        switch (expected)
        {
            case EXPECT_PORT_NUMBER : {
                args_.port_ = ToInt(arg);
                expected = EXPECT_NONE;
            } break;

            case EXPECT_NONE : {
                if (arg == "--port") expected = EXPECT_PORT_NUMBER;
                else
                {
                    ErrorExit("Unknown option " + arg);
                }
            } break;
        }
    }

    if (expected != EXPECT_NONE)
    {
        ErrorExit("Missing argument to command line option");
    }
}

// ----------------------------------------------------------------------------
void ServerApplication::LoadScene()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>(LOCAL);
    scene_->CreateComponent<PhysicsWorld>(LOCAL);

    planet_ = scene_->CreateChild("Planet");
    planetXML_ = cache->GetResource<XMLFile>("Prefabs/ShizzlePlanet.xml");
    planet_->LoadXML(planetXML_->GetRoot());
}

// ----------------------------------------------------------------------------
void ServerApplication::SubscribeToEvents()
{
    SubscribeToEvent(E_USERJOINED, URHO3D_HANDLER(ServerApplication, HandleUserJoined));
    SubscribeToEvent(E_USERLEFT, URHO3D_HANDLER(ServerApplication, HandleUserLeft));
    SubscribeToEvent(E_PLAYERCREATE, URHO3D_HANDLER(ServerApplication, HandlePlayerCreate));
    SubscribeToEvent(E_PLAYERDESTROY, URHO3D_HANDLER(ServerApplication, HandlePlayerDestroy));
    SubscribeToEvent(E_FILECHANGED, URHO3D_HANDLER(ServerApplication, HandleFileChanged));
}

// ----------------------------------------------------------------------------
void ServerApplication::HandleUserJoined(StringHash eventType, VariantMap& eventData)
{
    using namespace UserJoined;

    uint32_t guid = eventData[P_GUID].GetUInt();
    const User* user = GetSubsystem<UserRegistry>()->GetUser(guid);

    assert(user->GetConnection() != nullptr);
    user->GetConnection()->SetScene(scene_);

    // Send ship create event here for now. May have a spawning subsystem later
    // that determines where and when players are spawned
    VariantMap data;
    data[PlayerCreate::P_GUID] = user->GetGUID();
    data[PlayerCreate::P_PIVOTROTATION] = Quaternion::IDENTITY;  // whatever lol
    GetSubsystem<Network>()->BroadcastRemoteEvent(E_PLAYERCREATE, true, data);
    SendEvent(E_PLAYERCREATE, data);
}

// ----------------------------------------------------------------------------
void ServerApplication::HandleUserLeft(StringHash eventType, VariantMap& eventData)
{
    using namespace UserLeft;

    // Send ship destroy event here for now. May have a spawning subsystem
    // later
    VariantMap data;
    data[PlayerDestroy::P_GUID] = eventData[P_GUID].GetInt();
    GetSubsystem<Network>()->BroadcastRemoteEvent(E_PLAYERDESTROY, true, data);
    SendEvent(E_PLAYERDESTROY, data);
}

// ----------------------------------------------------------------------------
void ServerApplication::HandlePlayerCreate(StringHash eventType, VariantMap& eventData)
{
    using namespace PlayerCreate;

    User::GUID guid = eventData[P_GUID].GetUInt();
    assert(shipNodes_.Find(guid) == shipNodes_.End());
    User* user = GetSubsystem<UserRegistry>()->GetUser(guid);

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* config = cache->GetResource<XMLFile>("Prefabs/ServerShip.xml");

    Node* node = scene_->CreateChild("", LOCAL);
    node->LoadXML(config->GetRoot());
    node->SetRotation(eventData[P_PIVOTROTATION].GetQuaternion());
    node->GetChild("Ship")->GetComponent<ServerShipState>()->SetUser(user);

    shipNodes_[guid] = node;
}

// ----------------------------------------------------------------------------
void ServerApplication::HandlePlayerDestroy(StringHash eventType, VariantMap& eventData)
{
    using namespace PlayerDestroy;

    User::GUID guid = eventData[P_GUID].GetUInt();

    assert(shipNodes_.Find(guid) != shipNodes_.End());

    shipNodes_[guid]->Remove();
    shipNodes_.Erase(guid);
}

// ----------------------------------------------------------------------------
void ServerApplication::HandleFileChanged(StringHash eventType, VariantMap& eventData)
{
    using namespace FileChanged;

    if (eventData[P_RESOURCENAME].GetString() == planetXML_->GetName())
    {
        planet_->LoadXML(planetXML_->GetRoot());
    }
}

}
