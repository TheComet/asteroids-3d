#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Globals.hpp"
#include "Asteroids/Player/ShipController.hpp"
#include "Asteroids/Player/ActionState.hpp"

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
    angle_(0)
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
float ShipController::GetAngle() const
{
    return angle_;
}

// ----------------------------------------------------------------------------
void ShipController::SetAngle(float angle)
{
    angle_ = angle;
}

// ----------------------------------------------------------------------------
const Vector2& ShipController::GetVelocity() const
{
    return velocity_;
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
void ShipController::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    float dt = eventData[P_TIMESTEP].GetFloat();
    ActionState* state = GetComponent<ActionState>();
    if (state == nullptr)
        return;

    // Update Y rotation of player model depending on left/right input
    angle_ += (state->GetRight() - state->GetLeft()) * shipConfig_.rotationSpeed_ * dt;
    if (angle_ > 360) angle_ -= 360;
    if (angle_ < 0) angle_ += 360;
    node_->SetRotation(Quaternion(0, angle_, 0));

    if (state->IsThrusting())
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
        decay = Min(decay, vlength);  // In case of very large timesteps
        float angleOfTrajectory = Atan2(velocity_.y_, velocity_.x_);
        velocity_.x_ -= Cos(angleOfTrajectory) * decay;
        velocity_.y_ -= Sin(angleOfTrajectory) * decay;
    }

    UpdatePosition(velocity_, dt);
    UpdatePlanetHeight();
    node_->SetPosition(Vector3(0, GetOffsetFromPlanetCenter(), 0));
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
