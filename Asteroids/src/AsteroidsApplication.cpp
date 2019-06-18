#include "Asteroids/AsteroidsApplication.hpp"
#include "Asteroids/Bullet.hpp"
#include "Asteroids/DebugTextScroll.hpp"
#include "Asteroids/Globals.hpp"
#include "Asteroids/InputActionMapper.hpp"
#include "Asteroids/OrbitingCameraController.hpp"
#include "Asteroids/Player.hpp"

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
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
AsteroidsApplication::AsteroidsApplication(Context* context) :
    Application(context),
    drawPhyGeometry_(false)
{
}

// ----------------------------------------------------------------------------
void AsteroidsApplication::Setup()
{
    engineParameters_["FullScreen"]      = false;
    engineParameters_["WindowResizable"] = true;
    engineParameters_["VSync"]           = true;
    engineParameters_["Multisample"]     = 2;
}

// ----------------------------------------------------------------------------
void AsteroidsApplication::Start()
{
    RegisterStuff();
#if defined(DEBUG)
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
    scene_->CreateComponent<Octree>();
    //scene_->CreateComponent<PhysicsWorld>();

#if defined(DEBUG)
    scene_->CreateComponent<DebugRenderer>();
#endif

    Node* planetNode = scene_->CreateChild("Planet");
    StaticModel* planetModel = planetNode->CreateComponent<StaticModel>();
    planetModel->SetModel(cache->GetResource<Model>("Models/TestPlanet.mdl"));
    planetModel->SetMaterial(cache->GetResource<Material>("Materials/Wireframe.xml"));
    RigidBody* planetBody = planetNode->CreateComponent<RigidBody>();
    planetBody->SetMass(0);
    planetBody->SetCollisionLayer(COLLISION_MASK_PLANET_SURFACE);
    CollisionShape* planetCollision = planetNode->CreateComponent<CollisionShape>();
    planetCollision->SetTriangleMesh(cache->GetResource<Model>("Models/TestPlanet.mdl"));

    Player* player = Player::Create(scene_);

    Node* lightNode = scene_->CreateChild("Light");
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetCastShadows(true);

    Node* cameraNode = player->GetNode()->GetParent()->CreateChild("Camera");
    cameraNode->SetRotation(Quaternion(90, 0, 0));
    Camera* camera = cameraNode->CreateComponent<Camera>();
    OrbitingCameraController* cameraController = cameraNode->CreateComponent<OrbitingCameraController>();
    cameraController->SetDistance(90);
    cameraController->SetTrackNode(player->GetNode());

    Renderer* renderer = GetSubsystem<Renderer>();
    Viewport* viewport = new Viewport(context_, scene_, camera);
    viewport->SetDrawDebug(true);
    renderer->SetViewport(0, viewport);

    SubscribeToEvents();
}

// ----------------------------------------------------------------------------
void AsteroidsApplication::Stop()
{
}

// ----------------------------------------------------------------------------
void AsteroidsApplication::RegisterStuff()
{
#if defined(DEBUG)
    DebugTextScroll::RegisterSubsystem(context_);
    GetSubsystem<DebugTextScroll>()->SetTextCount(40);
    GetSubsystem<DebugTextScroll>()->SetTimeout(10);
#endif

    Bullet::RegisterObject(context_);
    InputActionMapper::RegisterObject(context_);
    OrbitingCameraController::RegisterObject(context_);
    Player::RegisterObject(context_);
}

// ----------------------------------------------------------------------------
void AsteroidsApplication::CreateDebugHud()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
    debugHud_ = engine_->CreateDebugHud();
    if (debugHud_)
        debugHud_->SetDefaultStyle(style);
}

// ----------------------------------------------------------------------------
void AsteroidsApplication::SubscribeToEvents()
{
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(AsteroidsApplication, HandleKeyDown));
    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(AsteroidsApplication, HandlePostRenderUpdate));
}

// ----------------------------------------------------------------------------
void AsteroidsApplication::HandleKeyDown(StringHash eventType, VariantMap& eventData)
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
void AsteroidsApplication::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
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

}
