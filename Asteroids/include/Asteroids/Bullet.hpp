#pragma once

#include "Asteroids/Config.hpp"
#include "Asteroids/SurfaceObject.hpp"

namespace Asteroids {

class ASTEROIDS_PUBLIC_API Bullet : public SurfaceObject
{
    URHO3D_OBJECT(Bullet, SurfaceObject)

public:
    Bullet(Urho3D::Context* context);
    static Bullet* Create(Urho3D::Scene* scene, Urho3D::Quaternion pivotRotation, float angle);
    static void Destroy(Bullet* bullet);

    void CalculateVelocity();
    void SetLife(float life);

private:
    void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::Vector2 velocity_;
    float life_;
};

}
