#include "Asteroids/Player/OrbitingCameraController.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Scene/Node.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
OrbitingCameraController::OrbitingCameraController(Context* context) :
    Component(context),
    distance_(1)
{
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
void OrbitingCameraController::SetDistance(float distance)
{
    distance_ = distance;
}

// ----------------------------------------------------------------------------
void OrbitingCameraController::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    float dt = eventData[P_TIMESTEP].GetFloat();
    const float lookAheadDistance = 5;
    const float smoothSpeed = 2;

    if (trackNode_.Expired())
    {
        SetTrackNode(nullptr);
        return;
    }

    const Vector3& currentPos = node_->GetWorldPosition();
    Vector3 targetPos = trackNode_->GetWorldPosition() + trackNode_->GetWorldDirection() * lookAheadDistance + trackNode_->GetWorldPosition().Normalized() * distance_;
    Vector3 direction = targetPos - currentPos;
    float len = direction.Length();
    direction /= len;
    len = Clamp(smoothSpeed * len * dt, -len, len);

    node_->SetWorldPosition(currentPos + direction * len);
}

}
