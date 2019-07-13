#include "Asteroids/Player/OrbitingCameraController.hpp"
#include "Asteroids/AsteroidsLib.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/ResourceEvents.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
OrbitingCameraController::OrbitingCameraController(Context* context) :
    Component(context)
{
}


// ----------------------------------------------------------------------------
void OrbitingCameraController::RegisterObject(Context* context)
{
    context->RegisterFactory<OrbitingCameraController>(ASTEROIDS_CATEGORY);

    URHO3D_ACCESSOR_ATTRIBUTE("Cam Config", GetConfigAttr, SetConfigAttr, ResourceRef, ResourceRef(XMLFile::GetTypeStatic()), AM_DEFAULT);
}

// ----------------------------------------------------------------------------
void OrbitingCameraController::SetConfig(XMLFile* config)
{
    if (configFile_)
    {
        UnsubscribeFromEvent(E_UPDATE);
        UnsubscribeFromEvent(E_FILECHANGED);
    }

    configFile_ = config;

    if (configFile_)
    {
        SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(OrbitingCameraController, HandleUpdate));
        SubscribeToEvent(E_FILECHANGED, URHO3D_HANDLER(OrbitingCameraController, HandleFileChanged));
        ParseCamConfig();
    }
}

// ----------------------------------------------------------------------------
ResourceRef OrbitingCameraController::GetConfigAttr() const
{
    return GetResourceRef(configFile_, XMLFile::GetTypeStatic());
}

// ----------------------------------------------------------------------------
void OrbitingCameraController::SetConfigAttr(const ResourceRef& value)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    SetConfig(cache->GetResource<XMLFile>(value.name_));
}

// ----------------------------------------------------------------------------
void OrbitingCameraController::ParseCamConfig()
{
    XMLElement cam = configFile_->GetRoot();
    for (XMLElement param = cam.GetChild("param"); param; param = param.GetNext("param"))
    {
        String paramName = param.GetAttribute("name");
        if      (paramName == "camDistance") camConfig_.distance_ = param.GetFloat("value");
        else if (paramName == "lookAheadDistance")  camConfig_.lookAhead_ = param.GetFloat("value");
        else if (paramName == "camSmoothSpeed")   camConfig_.smooth_ = param.GetFloat("value");
        else
        {
            URHO3D_LOGERRORF("Unknown parameter \"%s\" while reading config file \"%s\"", paramName.CString(), configFile_->GetName().CString());
        }
    }
}

// ----------------------------------------------------------------------------
void OrbitingCameraController::HandleFileChanged(StringHash eventType, VariantMap& eventData)
{
    using namespace FileChanged;

    if (configFile_->GetName() == eventData[P_RESOURCENAME].GetString())
    {
        ParseCamConfig();
    }
}

// ----------------------------------------------------------------------------
void OrbitingCameraController::SetTrackNode(Node* nodeToTrack)
{
    if (trackNode_)
    {
        UnsubscribeFromEvent(E_UPDATE);
    }

    trackNode_ = nodeToTrack;

    if (trackNode_)
    {
        SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(OrbitingCameraController, HandleUpdate));
    }
}

// ----------------------------------------------------------------------------
void OrbitingCameraController::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    float dt = eventData[P_TIMESTEP].GetFloat();

    if (trackNode_.Expired())
    {
        SetTrackNode(nullptr);
        return;
    }

    const Vector3& camPos = node_->GetWorldPosition();
    const Quaternion& camDir = node_->GetWorldRotation();
    const Vector3& trackPos = trackNode_->GetWorldPosition();
    const Vector3& trackDir = trackNode_->GetWorldDirection();

    /*
    float r = trackPos.Length() + camConfig_.distance_;
    float theta = Acos(trackPos.z_/r);
    float phi = Atan2(trackPos.y_, trackPos.x_);
    URHO3D_LOGERRORF("%f %f %f", r,theta,phi);
    
    node_->SetWorldPosition(Vector3(
        r*Sin(theta)*Cos(phi),
        r*Sin(theta)*Sin(phi),
        r*Cos(theta)));
    */
    
    float r =  trackPos.Length();    
    //URHO3D_LOGERRORF("%f %f %f", trackDir.x_,trackDir.y_,trackDir.z_);
    node_->SetTransform(trackPos/r*(r+camConfig_.distance_),
                        Quaternion(Vector3(0,0,1).Normalized(), -trackPos.Normalized()));
    /*    
    float r =  trackPos.Length();    
    URHO3D_LOGERRORF("POS %f %f %f", trackPos.x_,trackPos.y_,trackPos.z_);
    URHO3D_LOGERRORF("DIR %f %f %f", trackDir.x_,trackDir.y_,trackDir.z_);
    node_->SetTransform(trackPos/r*(r+camConfig_.distance_),
                        Quaternion(Vector3(0,0,1).Normalized(), -Vector3(0,1,0).Normalized()));
    */
    
    //Vector3 targetPos = trackNode_->GetWorldPosition() + trackNode_->GetWorldDirection() * lookAheadDistance + trackNode_->GetWorldPosition().Normalized() * distance_;
    //Vector3 direction = targetPos - currentPos;
    //float len = direction.Length();
    //direction /= len;
    //len = Clamp(smoothSpeed * len * dt, -len, len);
    //
}

}
