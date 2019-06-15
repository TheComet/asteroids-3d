#include "Asteroids/Player.hpp"
#include "Asteroids/InputActionMapper.hpp"
#include "Asteroids/InputActionMapperEvents.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
Player::Player(Context* context) :
    Component(context)
{
}

// ----------------------------------------------------------------------------
void Player::RegisterObject(Context* context)
{
    context->RegisterFactory<Player>("Asteroids");
}

// ----------------------------------------------------------------------------
void Player::SetInputActionMapper(InputActionMapper* mapper)
{
    if (mapper_)
    {
        UnsubscribeFromEvent(E_ACTIONWARP);
        UnsubscribeFromEvent(E_ACTIONUSEITEM);
        UnsubscribeFromEvent(E_UPDATE);
    }

    mapper_ = mapper;

    if (mapper_)
    {
        SubscribeToEvent(mapper_, E_ACTIONWARP, URHO3D_HANDLER(Player, HandleActionWarp));
        SubscribeToEvent(mapper_, E_ACTIONUSEITEM, URHO3D_HANDLER(Player, HandleActionUseItem));
        SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Player, HandleUpdate));
    }
}

// ----------------------------------------------------------------------------
void Player::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;
    float dt = eventData[P_TIMESTEP].GetFloat();

    if (mapper_->IsFiring())
        URHO3D_LOGDEBUG("Firing");
    if (mapper_->IsThrusting())
        URHO3D_LOGDEBUG("Thrusting");
    if (mapper_->GetLeft() > 0.0)
        URHO3D_LOGDEBUGF("Left: %f", mapper_->GetLeft());
    if (mapper_->GetRight() > 0.0)
        URHO3D_LOGDEBUGF("Right: %f", mapper_->GetRight());

}

// ----------------------------------------------------------------------------
void Player::HandleActionWarp(StringHash eventType, VariantMap& eventData)
{
    URHO3D_LOGDEBUG("Warp action");
}

// ----------------------------------------------------------------------------
void Player::HandleActionUseItem(StringHash eventType, VariantMap& eventData)
{
    URHO3D_LOGDEBUG("UseItem action");
}

}
