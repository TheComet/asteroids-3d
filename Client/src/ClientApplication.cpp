#include "Client/ClientApplication.hpp"
#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Bullet.hpp"
#include "Asteroids/DebugTextScroll.hpp"
#include "Asteroids/Globals.hpp"
#include "Asteroids/InputActionMapper.hpp"
#include "Asteroids/OrbitingCameraController.hpp"
#include "Asteroids/Player.hpp"

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
#if defined(DEBUG)
    DebugTextScroll::RegisterSubsystem(context_);
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

#if defined(DEBUG)
    scene_->CreateComponent<DebugRenderer>();
#endif

    Node* cameraPivotNode = scene_->CreateChild("CameraPivot", LOCAL);
    Node* cameraNode = cameraPivotNode->CreateChild("Camera", LOCAL);
    cameraNode->SetPosition(Vector3(0, 160, 0));
    cameraNode->SetRotation(Quaternion(90, 0, 0));
    Camera* camera = cameraNode->CreateComponent<Camera>();
    /*OrbitingCameraController* cameraController = cameraNode->CreateComponent<OrbitingCameraController>();
    cameraController->SetDistance(90);
    cameraController->SetTrackNode(player->GetNode());*/

    Renderer* renderer = GetSubsystem<Renderer>();
    Viewport* viewport = new Viewport(context_, scene_, camera);
    viewport->SetDrawDebug(true);
    renderer->SetViewport(0, viewport);

    SubscribeToEvents();

    Network* network = GetSubsystem<Network>();
    network->Connect("127.0.0.1", 6666, scene_);
}

// ----------------------------------------------------------------------------
void ClientApplication::Stop()
{
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
    SubscribeToEvent(E_SERVERCONNECTED, URHO3D_HANDLER(ClientApplication, HandleServerConnected));
    SubscribeToEvent(E_SERVERDISCONNECTED, URHO3D_HANDLER(ClientApplication, HandleServerDisconnected));
    SubscribeToEvent(E_CONNECTFAILED, URHO3D_HANDLER(ClientApplication, HandleConnectFailed));
}

// ----------------------------------------------------------------------------
void ClientApplication::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyDown;

    int key = eventData[P_KEY].GetInt();

    // Exit game
    if (key == KEY_ESCAPE)
        engine_->Exit();

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
void ClientApplication::HandleServerConnected(StringHash eventType, VariantMap& eventData)
{
    using namespace ServerConnected;

    URHO3D_LOGDEBUGF("Connected to server");
}

// ----------------------------------------------------------------------------
void ClientApplication::HandleServerDisconnected(StringHash eventType, VariantMap& eventData)
{
    using namespace ServerDisconnected;

    URHO3D_LOGDEBUGF("Disconnected from server");
}

// ----------------------------------------------------------------------------
void ClientApplication::HandleConnectFailed(StringHash eventType, VariantMap& eventData)
{
    using namespace ConnectFailed;
}

}
