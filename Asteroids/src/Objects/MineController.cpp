#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Objects/MineController.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Scene/Node.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
MineController::MineController(Context* context) :
    SurfaceObject(context),
    deceleration_(0),
    life_(std::numeric_limits<float>::max())
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MineController, HandleUpdate));
}

// ----------------------------------------------------------------------------
void MineController::RegisterObject(Context* context)
{
    context->RegisterFactory<MineController>(ASTEROIDS_CATEGORY);
}

// ----------------------------------------------------------------------------
const Vector2& MineController::GetVelocity() const
{
    return velocity_;
}

// ----------------------------------------------------------------------------
void MineController::SetVelocity(const Vector2& velocity)
{
    velocity_ = velocity;
    node_->SetRotation(Quaternion(0, 2 * M_PI * Random(), 0));
}

// ----------------------------------------------------------------------------
void MineController::SetDeceleration(float deceleration)
{
    deceleration_ = deceleration;
}

// ----------------------------------------------------------------------------
void MineController::SetLife(float life)
{
    life_ = life;
}

// ----------------------------------------------------------------------------
void MineController::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;
    float dt = eventData[P_TIMESTEP].GetFloat();

    // Decelerate mine until it comes to a halt
    float vlength = velocity_.Length();
    float decay = vlength * dt * deceleration_;
    decay = Min(decay, vlength);  // In case of very large timesteps
    float angleOfTrajectory = Atan2(velocity_.y_, velocity_.x_);
    velocity_.x_ -= Cos(angleOfTrajectory) * decay;
    velocity_.y_ -= Sin(angleOfTrajectory) * decay;

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
