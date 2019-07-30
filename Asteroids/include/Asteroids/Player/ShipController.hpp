#pragma once

#include "Asteroids/Config.hpp"
#include "Asteroids/Objects/SurfaceObject.hpp"

namespace Urho3D
{
    class XMLFile;
}

namespace Asteroids {

class ActionState;

class ASTEROIDS_PUBLIC_API ShipController : public SurfaceObject
{
    URHO3D_OBJECT(ShipController, SurfaceObject)

public:
    ShipController(Urho3D::Context* context);

    static void RegisterObject(Urho3D::Context* context);

    void SetConfig(Urho3D::XMLFile* config);

    Urho3D::ResourceRef GetConfigAttr() const;
    void SetConfigAttr(const Urho3D::ResourceRef& value);

    float GetAngle() const;
    void SetAngle(float angle);

    const Urho3D::Vector2& GetVelocity() const;

private:
    void SubscribeToEvents();
    void ParseShipConfig();
    void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    struct ShipConfig
    {
        float rotationSpeed_;
        float acceleration_;
        float maxVelocity_;
        float velocityDecay_;
        float phaserCooldown_;
    } shipConfig_;

    Urho3D::SharedPtr<Urho3D::XMLFile> configFile_;
    Urho3D::Vector2 velocity_;
    float angle_;
};

}
