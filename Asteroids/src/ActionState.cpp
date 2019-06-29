#include "Asteroids/ActionState.hpp"
#include "Asteroids/ActionStateEvents.hpp"
#include "Asteroids/AsteroidsLib.hpp"

#include <Urho3D/Core/Context.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
ActionState::ActionState(Urho3D::Context* context) :
    Component(context),
    state_({{0}})
{
}

// ----------------------------------------------------------------------------
void ActionState::RegisterObject(Urho3D::Context* context)
{
    context->RegisterFactory<ActionState>(ASTEROIDS_CATEGORY);
}

// ----------------------------------------------------------------------------
uint16_t ActionState::GetState() const
{
    return state_.u16;
}

// ----------------------------------------------------------------------------
void ActionState::SetState(uint16_t newState)
{
    VariantMap& eventData = GetEventDataMap();
    uint16_t posEdge = newState & ~state_.u16;

    if (posEdge & 0x4000) SendEvent(E_ACTIONWARP, eventData);
    if (posEdge & 0x8000) SendEvent(E_ACTIONUSEITEM, eventData);

    state_.u16 = newState;
}

// ----------------------------------------------------------------------------
float ActionState::GetLeft() const
{
    return float(state_.data.left) / 0x3F;
}

// ----------------------------------------------------------------------------
void ActionState::SetLeft(float value)
{
    state_.data.left = unsigned(value * 0x3F);  // 6 bits of range
}

// ----------------------------------------------------------------------------
float ActionState::GetRight() const
{
    return float(state_.data.right) / 0x3F;
}

// ----------------------------------------------------------------------------
void ActionState::SetRight(float value)
{
    state_.data.right = unsigned(value * 0x3F);  // 6 bits of range
}

// ----------------------------------------------------------------------------
bool ActionState::IsThrusting() const
{
    return (state_.data.thrust == 1);
}

// ----------------------------------------------------------------------------
void ActionState::SetThrusting(bool enable)
{
    state_.data.thrust = enable;
}

// ----------------------------------------------------------------------------
bool ActionState::IsFiring() const
{
    return (state_.data.fire == 1);
}

// ----------------------------------------------------------------------------
void ActionState::SetFiring(bool enable)
{
    state_.data.fire = enable;
}

// ----------------------------------------------------------------------------
void ActionState::SetWarp(bool enable)
{
    if (enable && state_.data.warp == false)
        SendEvent(E_ACTIONWARP, GetEventDataMap());
    state_.data.warp = enable;
}

// ----------------------------------------------------------------------------
void ActionState::SetUseItem(bool enable)
{
    if (enable && state_.data.useItem == false)
        SendEvent(E_ACTIONUSEITEM, GetEventDataMap());
    state_.data.useItem = enable;
}

}
