#include "Server/ServerApplication.hpp"
#include "Server/SignalHandler.hpp"
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
    context_->RegisterSubsystem<SignalHandler>();
#if defined(DEBUG)
    GetSubsystem<Log>()->SetLevel(LOG_DEBUG);
#endif

    // Configure resource cache to auto-reload resources when they change on
    // the filesystem
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    cache->SetAutoReloadResources(true);

    SubscribeToEvents();

    Network* network = GetSubsystem<Network>();
    network->StartServer(6666);

    LoadScene();
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
}

// ----------------------------------------------------------------------------
void ServerApplication::SubscribeToEvents()
{
    SubscribeToEvent(E_CLIENTCONNECTED, URHO3D_HANDLER(ServerApplication, HandleClientConnected));
    SubscribeToEvent(E_CLIENTDISCONNECTED, URHO3D_HANDLER(ServerApplication, HandleClientDisconnected));
    SubscribeToEvent(E_CLIENTIDENTITY, URHO3D_HANDLER(ServerApplication, HandleClientIdentity));
    SubscribeToEvent(E_CLIENTSCENELOADED, URHO3D_HANDLER(ServerApplication, HandleClientSceneLoaded));
}

// ----------------------------------------------------------------------------
void ServerApplication::HandleClientConnected(StringHash eventType, VariantMap& eventData)
{
    using namespace ClientConnected;

    Connection* connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    connection->SetScene(scene_);
}

// ----------------------------------------------------------------------------
void ServerApplication::HandleClientDisconnected(StringHash eventType, VariantMap& eventData)
{
    using namespace ClientDisconnected;
}

// ----------------------------------------------------------------------------
void ServerApplication::HandleClientIdentity(StringHash eventType, VariantMap& eventData)
{
    using namespace ClientIdentity;
}

// ----------------------------------------------------------------------------
void ServerApplication::HandleClientSceneLoaded(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    URHO3D_LOGDEBUGF("Client scene loaded");
}

}
