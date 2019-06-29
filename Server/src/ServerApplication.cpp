#include "Server/ServerApplication.hpp"
#include "Server/SignalHandler.hpp"
#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Bullet.hpp"
#include "Asteroids/DebugTextScroll.hpp"
#include "Asteroids/Globals.hpp"
#include "Asteroids/OrbitingCameraController.hpp"

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
    LoadScene();

    Network* network = GetSubsystem<Network>();
    network->StartServer(6666);
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
