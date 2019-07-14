#include "Client/ClientApplication.hpp"
#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Menu/Menu.hpp"
#include "Asteroids/Menu/MenuEvents.hpp"
#include "Asteroids/Player/PlayerEvents.hpp"
#include "Asteroids/Player/DeviceInputMapper.hpp"
#include "Asteroids/Player/OrbitingCameraController.hpp"
#include "Asteroids/Player/ClientLocalShipState.hpp"
#include "Asteroids/Player/ClientRemoteShipState.hpp"
#include "Asteroids/Player/ShipController.hpp"
#include "Asteroids/UserRegistry/UserRegistry.hpp"
#include "Asteroids/UserRegistry/UserRegistryEvents.hpp"
#include "Asteroids/UserRegistry/ClientUserRegistry.hpp"
#include "Asteroids/Util/DebugTextScroll.hpp"

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/UI.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
ClientApplication::ClientApplication(Context* context) :
    Application(context),
    drawPhyGeometry_(false)
{
}

// ----------------------------------------------------------------------------
void ClientApplication::Setup()
{
    ParseArgs();

    engineParameters_[EP_LOG_NAME]         = "asteroids-client.log";
    engineParameters_[EP_FULL_SCREEN]      = false;
    engineParameters_[EP_WINDOW_RESIZABLE] = true;
    engineParameters_[EP_VSYNC]            = true;
    engineParameters_[EP_MULTI_SAMPLE]     = 2;
}

// ----------------------------------------------------------------------------
void ClientApplication::Start()
{
    RegisterObjectFactories(context_);
    RegisterRemoteNetworkEvents(context_);

    context_->RegisterSubsystem<ClientUserRegistry>();
    context_->RegisterSubsystem<UserRegistry>();
    context_->RegisterSubsystem<Menu>();

#if defined(DEBUG)
    context_->RegisterSubsystem<DebugTextScroll>();
    GetSubsystem<DebugTextScroll>()->SetTextCount(40);
    GetSubsystem<DebugTextScroll>()->SetTimeout(10);
    CreateDebugHud();
    GetSubsystem<Log>()->SetLevel(LOG_DEBUG);
#endif

    // Configure resource cache to auto-reload resources when they change on
    // the filesystem
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    cache->SetAutoReloadResources(true);

    // UI style
    XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
    UI* ui = GetSubsystem<UI>();
    if (style && ui)
        ui->GetRoot()->SetDefaultStyle(style);

    // Mouse should be visible by default
    GetSubsystem<Input>()->SetMouseVisible(true);

    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>(LOCAL);
    scene_->CreateComponent<PhysicsWorld>(LOCAL);

#if defined(DEBUG)
    scene_->CreateComponent<DebugRenderer>();
#endif

    Node* cameraPivotNode = scene_->CreateChild("CameraPivot", LOCAL);
    cameraNode_ = cameraPivotNode->CreateChild("Camera", LOCAL);
    cameraNode_->SetPosition(Vector3(0, 160, 0));
    cameraNode_->SetRotation(Quaternion(90, 0, 0));
    Camera* camera = cameraNode_->CreateComponent<Camera>();
    OrbitingCameraController* cameraController = cameraNode_->CreateComponent<OrbitingCameraController>();
    cameraController->SetConfig(cache->GetResource<XMLFile>("Config/Camera.xml"));

    Renderer* renderer = GetSubsystem<Renderer>();
    Viewport* viewport = new Viewport(context_, scene_, camera);
    viewport->SetDrawDebug(true);
    renderer->SetViewport(0, viewport);

    SubscribeToEvents();

    GetSubsystem<Menu>()->StartMainMenu();
}

// ----------------------------------------------------------------------------
void ClientApplication::Stop()
{
    GetSubsystem<Network>()->Disconnect();
}

// ----------------------------------------------------------------------------
void ClientApplication::ParseArgs()
{
    enum Expect
    {
        EXPECT_NONE,
        EXPECT_NAME
    } expected = EXPECT_NONE;

    for (const auto& arg : GetArguments())
    {
        switch (expected)
        {
            case EXPECT_NAME : {
                args_.username_ = arg;
                expected = EXPECT_NONE;
            } break;

            case EXPECT_NONE : {
                if (arg == "--username") expected = EXPECT_NAME;
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
void ClientApplication::CreateDebugHud()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
    debugHud_ = engine_->CreateDebugHud();
    if (debugHud_)
        debugHud_->SetDefaultStyle(style);
}

// ----------------------------------------------------------------------------
void ClientApplication::SubscribeToEvents()
{
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(ClientApplication, HandleKeyDown));
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(ClientApplication, HandlePostRenderUpdate));
    SubscribeToEvent(E_MAINMENUQUIT, URHO3D_HANDLER(ClientApplication, HandleMainMenuQuit));
    SubscribeToEvent(E_CONNECTPROMPTREQUESTCONNECT, URHO3D_HANDLER(ClientApplication, HandleConnectPromptRequestConnect));
    SubscribeToEvent(E_CONNECTPROMPTREQUESTCANCEL, URHO3D_HANDLER(ClientApplication, HandleConnectPromptRequestCancel));
    SubscribeToEvent(E_PLAYERCREATE, URHO3D_HANDLER(ClientApplication, HandlePlayerCreate));
    SubscribeToEvent(E_PLAYERDESTROY, URHO3D_HANDLER(ClientApplication, HandlePlayerDestroy));
    SubscribeToEvent(E_REGISTERSUCCEEDED, URHO3D_HANDLER(ClientApplication, HandleRegisterSucceeded));
}

// ----------------------------------------------------------------------------
void ClientApplication::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyDown;

    int key = eventData[P_KEY].GetInt();

    if (key == KEY_F1)
        drawPhyGeometry_ = !drawPhyGeometry_;

    // Toggle debug HUD
    if (key == KEY_F2)
    {
        if (debugHud_->GetMode() == DEBUGHUD_SHOW_NONE)
            debugHud_->SetMode(DEBUGHUD_SHOW_ALL);
        else if (debugHud_->GetMode() == DEBUGHUD_SHOW_ALL)
            debugHud_->SetMode(DEBUGHUD_SHOW_MEMORY);
        else
            debugHud_->SetMode(DEBUGHUD_SHOW_NONE);
    }
}

// ----------------------------------------------------------------------------
void ClientApplication::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{
    DebugRenderer* r = scene_->GetComponent<DebugRenderer>();
    if (r == nullptr)
        return;

    if (drawPhyGeometry_)
    {
        PhysicsWorld* phy = scene_->GetComponent<PhysicsWorld>();
        if (phy)
            phy->DrawDebugGeometry(true);
    }
}

// ----------------------------------------------------------------------------
void ClientApplication::HandleMainMenuQuit(StringHash eventType, VariantMap& eventData)
{
    engine_->Exit();
}

// ----------------------------------------------------------------------------
void ClientApplication::HandleConnectPromptRequestConnect(StringHash eventType, VariantMap& eventData)
{
    using namespace ConnectPromptRequestConnect;

    GetSubsystem<ClientUserRegistry>()->TryRegister(
        eventData[P_USERNAME].GetString(),
        eventData[P_ADDRESS].GetString(),
        eventData[P_PORT].GetInt(),
        scene_
    );
}

// ----------------------------------------------------------------------------
void ClientApplication::HandleConnectPromptRequestCancel(StringHash eventType, VariantMap& eventData)
{
    GetSubsystem<Network>()->Disconnect();
}

// ----------------------------------------------------------------------------
void ClientApplication::HandlePlayerCreate(StringHash eventType, VariantMap& eventData)
{
    using namespace PlayerCreate;

    User::GUID guid = eventData[P_GUID].GetUInt();
    assert(shipNodes_.Find(guid) == shipNodes_.End());
    User* user = GetSubsystem<UserRegistry>()->GetUser(guid);

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* shipfab = cache->GetResource<XMLFile>(
        guid == myGuid_ ? "Prefabs/ClientLocalShip.xml" : "Prefabs/ClientRemoteShip.xml");

    Node* node = scene_->CreateChild("", LOCAL);
    node->LoadXML(shipfab->GetRoot());
    node->SetRotation(eventData[P_PIVOTROTATION].GetQuaternion());

    shipNodes_[guid] = node;

    if (guid == myGuid_)
    {
        node->GetChild("Ship")->GetComponent<ClientLocalShipState>()->SetUser(user);
        cameraNode_->GetComponent<OrbitingCameraController>()->SetTrackNode(node->GetChild("Ship"));
    }
    else
    {
        node->GetChild("Ship")->GetComponent<ClientRemoteShipState>()->SetUser(user);
    }
}

// ----------------------------------------------------------------------------
void ClientApplication::HandlePlayerDestroy(StringHash eventType, VariantMap& eventData)
{
    using namespace PlayerDestroy;

    User::GUID guid = eventData[P_GUID].GetUInt();

    assert(shipNodes_.Find(guid) != shipNodes_.End());

    shipNodes_[guid]->Remove();
    shipNodes_.Erase(guid);
}

// ----------------------------------------------------------------------------
void ClientApplication::HandleRegisterSucceeded(StringHash eventType, VariantMap& eventData)
{
    using namespace RegisterSucceeded;
    myGuid_ = eventData[P_GUID].GetInt();
}

}
