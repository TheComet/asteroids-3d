#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Objects/Bullet.hpp"
#include "Asteroids/Globals.hpp"
#include "Asteroids/Player/ShipController.hpp"
#include "Asteroids/Player/ActionState.hpp"
#include "Asteroids/Player/ActionStateEvents.hpp"

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
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
ShipController::ShipController(Context* context) :
    SurfaceObject(context),
    shipConfig_({0, 0, 0, 0}),
    angle_(0),
    fireActionCooldown_(0)
{
}

// ----------------------------------------------------------------------------
void ShipController::RegisterObject(Context* context)
{
    context->RegisterFactory<ShipController>(ASTEROIDS_CATEGORY);

    URHO3D_ACCESSOR_ATTRIBUTE("Ship Config", GetConfigAttr, SetConfigAttr, ResourceRef, ResourceRef(XMLFile::GetTypeStatic()), AM_DEFAULT);
}

// ----------------------------------------------------------------------------
void ShipController::SetConfig(XMLFile* config)
{
    if (configFile_)
    {
        UnsubscribeFromEvent(E_UPDATE);
        UnsubscribeFromEvent(E_FILECHANGED);
    }

    configFile_ = config;

    if (configFile_)
    {
        SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(ShipController, HandleUpdate));
        SubscribeToEvent(E_FILECHANGED, URHO3D_HANDLER(ShipController, HandleFileChanged));
        ParseShipConfig();
    }
}

// ----------------------------------------------------------------------------
ResourceRef ShipController::GetConfigAttr() const
{
    return GetResourceRef(configFile_, XMLFile::GetTypeStatic());
}

// ----------------------------------------------------------------------------
void ShipController::SetConfigAttr(const ResourceRef& value)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    SetConfig(cache->GetResource<XMLFile>(value.name_));
}

// ----------------------------------------------------------------------------
void ShipController::ParseShipConfig()
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
bool ShipController::TryGetActionState()
{
    if (state_.NotNull())
    {
        UnsubscribeFromEvent(E_ACTIONWARP);
        UnsubscribeFromEvent(E_ACTIONUSEITEM);
        state_ = nullptr;
    }

    state_ = GetComponent<ActionState>();

    if (state_.NotNull())
    {
        SubscribeToEvent(state_, E_ACTIONWARP, URHO3D_HANDLER(ShipController, HandleActionWarp));
        SubscribeToEvent(state_, E_ACTIONUSEITEM, URHO3D_HANDLER(ShipController, HandleActionUseItem));
    }

    return state_.Expired();
}

// ----------------------------------------------------------------------------
void ShipController::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    float dt = eventData[P_TIMESTEP].GetFloat();

    if (state_.Expired() && TryGetActionState() == false)
        return;

    // Update Y rotation of player model depending on left/right input
    angle_ += (state_->GetRight() - state_->GetLeft()) * shipConfig_.rotationSpeed_ * dt;
    if (angle_ > 360) angle_ -= 360;
    if (angle_ < 0) angle_ += 360;
    node_->SetRotation(Quaternion(0, angle_, 0));

    if (state_->IsThrusting())
    {
        // Update player speed
        velocity_.x_ += Sin(angle_) * shipConfig_.acceleration_ * dt;
        velocity_.y_ += Cos(angle_) * shipConfig_.acceleration_ * dt;

        // Speed limit
        float currentSpeedSquared = velocity_.LengthSquared();
        if (currentSpeedSquared > shipConfig_.maxVelocity_ * shipConfig_.maxVelocity_)
            velocity_ *=  shipConfig_.maxVelocity_ / Sqrt(currentSpeedSquared);
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

    fireActionCooldown_ = Max(0.0, fireActionCooldown_ - dt);
    if (fireActionCooldown_ == 0.0 && state_->IsFiring())
    {
        fireActionCooldown_ = 0.2;
        Bullet::Create(GetScene(), node_->GetParent()->GetRotation(), angle_);
    }

    UpdatePosition(velocity_, dt);
    UpdatePlanetHeight();
}

// ----------------------------------------------------------------------------
void ShipController::HandleActionWarp(StringHash eventType, VariantMap& eventData)
{
    URHO3D_LOGDEBUG("Warp action");
}

// ----------------------------------------------------------------------------
void ShipController::HandleActionUseItem(StringHash eventType, VariantMap& eventData)
{
}

// ----------------------------------------------------------------------------
void ShipController::HandleFileChanged(StringHash eventType, VariantMap& eventData)
{
    using namespace FileChanged;

    if (configFile_->GetName() == eventData[P_RESOURCENAME].GetString())
    {
        ParseShipConfig();
    }
}

}
