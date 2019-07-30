#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Objects/BulletController.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
#include <limits>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
BulletController::BulletController(Context* context) :
    SurfaceObject(context),
    life_(std::numeric_limits<float>::max())
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(BulletController, HandleUpdate));
}

// ----------------------------------------------------------------------------
void BulletController::RegisterObject(Context* context)
{
    context->RegisterFactory<BulletController>(ASTEROIDS_CATEGORY);
}

// ----------------------------------------------------------------------------
void BulletController::SetVelocity(const Vector2& velocity)
{
    velocity_ = velocity;
    node_->SetRotation(Quaternion(0, Atan2(velocity.x_, velocity.y_), 0));
}

// ----------------------------------------------------------------------------
void BulletController::SetLife(float life)
{
    life_ = life;
}

// ----------------------------------------------------------------------------
const Vector2& BulletController::GetVelocity() const
{
    return velocity_;
}

// ----------------------------------------------------------------------------
void BulletController::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    float dt = eventData[P_TIMESTEP].GetFloat();

    UpdatePosition(velocity_, dt);
    UpdatePlanetHeight();
    node_->SetPosition(Vector3(0, GetOffsetFromPlanetCenter(), 0));

    life_ -= dt;
    if (life_ < 0)
    {
        node_->GetParent()->Remove();
    }
}

}
