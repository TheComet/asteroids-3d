#include "Asteroids/Bullet.hpp"
#include "Asteroids/Globals.hpp"
#include "Asteroids/Player.hpp"
#include "Asteroids/InputActionMapper.hpp"
#include "Asteroids/InputActionMapperEvents.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/ResourceEvents.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
Player::Player(Context* context) :
    SurfaceObject(context),
    shipConfig_({0, 0, 0, 0}),
    angle_(0),
    shootCooldown_(0)
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Player, HandleUpdate));
}

// ----------------------------------------------------------------------------
void Player::RegisterObject(Context* context)
{
    context->RegisterFactory<Player>("Asteroids");
}

// ----------------------------------------------------------------------------
Player* Player::Create(Scene* scene)
{
    ResourceCache* cache = scene->GetSubsystem<ResourceCache>();

    Node* playerPivotNode = scene->CreateChild("PlayerPivot");
    Node* playerModelNode = playerPivotNode->CreateChild("PlayerModel");
    playerModelNode->SetPosition(Vector3(0, 1, 0));

    InputActionMapper* mapper = playerModelNode->CreateComponent<InputActionMapper>();
    mapper->SetConfig(cache->GetResource<XMLFile>("Config/InputMap.xml"));

    StaticModel* playerModel = playerModelNode->CreateComponent<StaticModel>();
    playerModel->SetModel(cache->GetResource<Model>("Models/TestShip.mdl"));
    playerModel->SetMaterial(cache->GetResource<Material>("Materials/DefaultGrey.xml"));

    Player* player = playerModelNode->CreateComponent<Player>();
    player->ListenToMapper(mapper);
    player->SetConfig(cache->GetResource<XMLFile>("Config/Ship.xml"));
    player->UpdatePlanetHeight();

    return player;
}

// ----------------------------------------------------------------------------
void Player::Destroy(Player* player)
{
    player->GetNode()->GetParent()->Remove();
}

// ----------------------------------------------------------------------------
void Player::SetConfig(XMLFile* config)
{
    if (configFile_)
    {
        UnsubscribeFromEvent(E_FILECHANGED);
    }

    configFile_ = config;

    if (configFile_)
    {
        SubscribeToEvent(E_FILECHANGED, URHO3D_HANDLER(Player, HandleFileChanged));
        ReadShipConfig();
    }
}

// ----------------------------------------------------------------------------
void Player::ListenToMapper(InputActionMapper* mapper)
{
    if (mapper_)
    {
        UnsubscribeFromEvent(E_ACTIONWARP);
        UnsubscribeFromEvent(E_ACTIONUSEITEM);
    }

    mapper_ = mapper;

    if (mapper_)
    {
        SubscribeToEvent(mapper, E_ACTIONWARP, URHO3D_HANDLER(Player, HandleActionWarp));
        SubscribeToEvent(mapper, E_ACTIONUSEITEM, URHO3D_HANDLER(Player, HandleActionUseItem));
    }
}

// ----------------------------------------------------------------------------
void Player::ReadShipConfig()
{
    XMLElement ship = configFile_->GetRoot();
    for (XMLElement param = ship.GetChild("param"); param; param = param.GetNext("param"))
    {
        String paramName = param.GetAttribute("name");
        if      (paramName == "rotationSpeed") shipConfig_.rotationSpeed_ = param.GetFloat("value");
        else if (paramName == "acceleration")  shipConfig_.acceleration_ = param.GetFloat("value");
        else if (paramName == "maxVelocity")   shipConfig_.maxVelocity_ = param.GetFloat("value");
        else if (paramName == "velocityDecay") shipConfig_.velocityDecay_ = param.GetFloat("value");
        else
        {
            URHO3D_LOGERRORF("Unknown parameter \"%s\" while reading config file \"%s\"", paramName.CString(), configFile_->GetName().CString());
        }
    }
}

// ----------------------------------------------------------------------------
void Player::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    float dt = eventData[P_TIMESTEP].GetFloat();

    // Need input mapper to function
    if (mapper_.Expired())
    {
        ListenToMapper(nullptr);
        return;
    }

    // Update Y rotation of player model depending on left/right input
    angle_ += (mapper_->GetRight() - mapper_->GetLeft()) * shipConfig_.rotationSpeed_ * dt;
    if (angle_ > 360) angle_ -= 360;
    if (angle_ < 0) angle_ += 360;
    node_->SetRotation(Quaternion(0, angle_, 0));

    if (mapper_->IsThrusting())
    {
        // Update player speed
        velocity_.x_ += Sin(angle_) * shipConfig_.acceleration_ * dt;
        velocity_.y_ += Cos(angle_) * shipConfig_.acceleration_ * dt;

        // Speed limit
        float angleOfTrajectory = Atan2(velocity_.y_, velocity_.x_);
        float maxX = Cos(angleOfTrajectory) * shipConfig_.maxVelocity_;
        float maxZ = Sin(angleOfTrajectory) * shipConfig_.maxVelocity_;
        if (maxX > 0)
            velocity_.x_ = Min(velocity_.x_, maxX);
        else
            velocity_.x_ = Max(velocity_.x_, maxX);
        if (maxZ > 0)
            velocity_.y_ = Min(velocity_.y_, maxZ);
        else
            velocity_.y_ = Max(velocity_.y_, maxZ);
    }
    else
    {
        // Velocity decays over time
        float vlength = velocity_.Length();
        float decay = vlength * dt * shipConfig_.velocityDecay_;
        decay = Clamp(decay, -vlength, vlength);  // In case of very large timesteps
        float angleOfTrajectory = Atan2(velocity_.y_, velocity_.x_);
        velocity_.x_ -= Cos(angleOfTrajectory) * decay;
        velocity_.y_ -= Sin(angleOfTrajectory) * decay;
    }

    shootCooldown_ = Max(0.0, shootCooldown_ - dt);
    if (shootCooldown_ == 0.0 && mapper_->IsFiring())
    {
        shootCooldown_ = 0.2;
        Bullet::Create(GetScene(), node_->GetParent()->GetRotation(), angle_);
    }

    UpdatePosition(velocity_, dt);
    UpdatePlanetHeight();
}

// ----------------------------------------------------------------------------
void Player::HandleActionWarp(StringHash eventType, VariantMap& eventData)
{
    URHO3D_LOGDEBUG("Warp action");
}

// ----------------------------------------------------------------------------
void Player::HandleActionUseItem(StringHash eventType, VariantMap& eventData)
{
}

// ----------------------------------------------------------------------------
void Player::HandleFileChanged(StringHash eventType, VariantMap& eventData)
{
    using namespace FileChanged;

    if (configFile_->GetName() == eventData[P_RESOURCENAME].GetString())
    {
        ReadShipConfig();
    }
}

}
