#include "Asteroids/Globals.hpp"
#include "Asteroids/Objects/SurfaceObject.hpp"

#include <Urho3D/Math/Ray.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
SurfaceObject::SurfaceObject(Context* context) :
    Component(context),
    planetHeight_(1),
    surfaceOffset_(0)
{
}

// ----------------------------------------------------------------------------
float SurfaceObject::GetOffsetFromPlanetCenter() const
{
    return planetHeight_ + surfaceOffset_;
}

// ----------------------------------------------------------------------------
void SurfaceObject::UpdatePosition(const Vector2& localLinearVelocity, float dt)
{
    Node* pivot = node_->GetParent();
    Quaternion xrot(2 * M_PI * localLinearVelocity.y_ / planetHeight_ * dt, Vector3::RIGHT);
    Quaternion zrot(2 * M_PI * localLinearVelocity.x_ / planetHeight_ * dt, Vector3::BACK);
    pivot->Rotate(xrot * zrot);
}

// ----------------------------------------------------------------------------
void SurfaceObject::UpdatePlanetHeight()
{
    Scene* scene = GetScene();
    PhysicsWorld* phy = scene->GetComponent<PhysicsWorld>();
    if (scene == nullptr || phy == nullptr)
    {
        if (!scene) URHO3D_LOGWARNINGF("Scene is null");
        if (!phy) URHO3D_LOGWARNINGF("PhysicsWorld is null");
        planetHeight_ = 1;
        return;
    }

    const Vector3& playerPos = node_->GetWorldPosition();
    const Vector3& pivotPos  = node_->GetParent()->GetWorldPosition();
    Vector3 direction = (pivotPos - playerPos).Normalized();
    Vector3 origin = pivotPos - direction * MAX_PLANET_RADIUS;

    PhysicsRaycastResult result;
    phy->RaycastSingle(result, Ray(origin , direction), MAX_PLANET_RADIUS, COLLISION_MASK_PLANET_TERRAIN);
    if (result.body_)
        planetHeight_ = Max(1.0, (result.position_ - pivotPos).Length());
}

}
