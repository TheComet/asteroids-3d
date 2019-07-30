#pragma once

#include "Asteroids/Config.hpp"
#include "Asteroids/Objects/SurfaceObject.hpp"

namespace Asteroids {

class ASTEROIDS_PUBLIC_API BulletController : public SurfaceObject
{
    URHO3D_OBJECT(BulletController, SurfaceObject)

public:
    BulletController(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

    void SetVelocity(const Urho3D::Vector2& velocity);
    void SetLife(float life);
    const Urho3D::Vector2& GetVelocity() const;

private:
    void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::Vector2 velocity_;
    float life_;
};

}
