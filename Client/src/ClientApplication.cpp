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
ClientApplication::ClientApplication(Context* context) :
    Application(context),
    drawPhyGeometry_(false)
{
}

// ----------------------------------------------------------------------------
void ClientApplication::Setup()
{
    engineParameters_["FullScreen"]      = false;
    engineParameters_["WindowResizable"] = true;
    engineParameters_["VSync"]           = true;
    engineParameters_["Multisample"]     = 2;
}

// ----------------------------------------------------------------------------
void ClientApplication::Start()
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

    Node* planetTerrainNode = scene_->CreateChild("PlanetTerrain");
    StaticModel* planetTerrainModel = planetTerrainNode->CreateComponent<StaticModel>();
    planetTerrainModel->SetModel(cache->GetResource<Model>("Models/TestPlanetTerrain.mdl"));
    planetTerrainModel->SetMaterial(cache->GetResource<Material>("Materials/DefaultGrey.xml"));
    RigidBody* planetTerrainBody = planetTerrainNode->CreateComponent<RigidBody>();
    planetTerrainBody->SetMass(0);
    planetTerrainBody->SetCollisionLayer(COLLISION_MASK_PLANET_TERRAIN);
    CollisionShape* planetTerrainCollision = planetTerrainNode->CreateComponent<CollisionShape>();
    planetTerrainCollision->SetTriangleMesh(cache->GetResource<Model>("Models/TestPlanetTerrain.mdl"));

    Node* planetWallsNode = scene_->CreateChild("PlanetWalls");
    StaticModel* planetWallsModel = planetWallsNode->CreateComponent<StaticModel>();
    planetWallsModel->SetModel(cache->GetResource<Model>("Models/TestPlanetWalls.mdl"));
    planetWallsModel->SetMaterial(cache->GetResource<Material>("Materials/DefaultGrey.xml"));
    RigidBody* planetWallsBody = planetWallsNode->CreateComponent<RigidBody>();
    planetWallsBody->SetMass(0);
    planetWallsBody->SetCollisionLayer(COLLISION_MASK_PLANET_WALLS);
    CollisionShape* planetWallsCollision = planetWallsNode->CreateComponent<CollisionShape>();
    planetWallsCollision->SetTriangleMesh(cache->GetResource<Model>("Models/TestPlanetWalls.mdl"));

    Player* player = Player::Create(scene_);

    Node* lightNode1 = scene_->CreateChild("Light");
    lightNode1->SetRotation(Quaternion(270, 0, 0));
    Light* light1 = lightNode1->CreateComponent<Light>();
    light1->SetLightType(LIGHT_DIRECTIONAL);
    light1->SetCastShadows(true);
    light1->SetColor(Color(0.2, 0.2, 0.5));

    Node* lightNode2 = scene_->CreateChild("Light");
    lightNode2->SetRotation(Quaternion(135, 0, 0));
    Light* light2 = lightNode2->CreateComponent<Light>();
    light2->SetLightType(LIGHT_DIRECTIONAL);
    light2->SetCastShadows(true);
    light2->SetColor(Color(0.5, 0.2, 0.2));

    Node* lightNode3 = scene_->CreateChild("Light");
    lightNode3->SetRotation(Quaternion(135, 120, 0));
    Light* light3 = lightNode3->CreateComponent<Light>();
    light3->SetLightType(LIGHT_DIRECTIONAL);
    light3->SetCastShadows(true);
    light3->SetColor(Color(0.2, 0.5, 0.2));

    Node* lightNode4 = scene_->CreateChild("Light");
    lightNode4->SetRotation(Quaternion(135, 240, 0));
    Light* light4 = lightNode4->CreateComponent<Light>();
    light4->SetLightType(LIGHT_DIRECTIONAL);
    light4->SetCastShadows(true);
    light4->SetColor(Color(0.5, 0.5, 0.2));

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
void ClientApplication::Stop()
{
}

// ----------------------------------------------------------------------------
void ClientApplication::RegisterStuff()
{
#if defined(DEBUG)
    DebugTextScroll::RegisterSubsystem(context_);
    GetSubsystem<DebugTextScroll>()->SetTextCount(40);
    GetSubsystem<DebugTextScroll>()->SetTimeout(10);
#endif

    RegisterObjectFactories(context_);
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

}
