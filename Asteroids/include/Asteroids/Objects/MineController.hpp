#pragma once

#include "Asteroids/Config.hpp"
#include "Asteroids/Objects/SurfaceObject.hpp"

namespace Asteroids {

class MineController : public SurfaceObject
{
    URHO3D_OBJECT(MineController, SurfaceObject)

public:
    MineController(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

    const Urho3D::Vector2& GetVelocity() const;
    void SetVelocity(const Urho3D::Vector2& velocity);
    void SetDeceleration(float deceleration);
    void SetLife(float life);

private:
    void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::Vector2 velocity_;
    float deceleration_;
    float life_;
};

}
