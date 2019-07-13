#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Player/ActionState.hpp"
#include "Asteroids/Player/ActionStateEvents.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/IO/Deserializer.h>
#include <Urho3D/IO/Serializer.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
ActionState::ActionState(Urho3D::Context* context) :
    Component(context),
    inputState_({{0}})
{
}

// ----------------------------------------------------------------------------
void ActionState::RegisterObject(Urho3D::Context* context)
{
    context->RegisterFactory<ActionState>(ASTEROIDS_CATEGORY);
}

// ----------------------------------------------------------------------------
void ActionState::SetState(Data newState)
{
    VariantMap& eventData = GetEventDataMap();
    uint16_t posEdge = newState & ~inputState_.u16;

    if (posEdge & 0x4000) SendEvent(E_ACTIONWARP, eventData);
    if (posEdge & 0x8000) SendEvent(E_ACTIONUSEITEM, eventData);

    inputState_.u16 = newState;
}

// ----------------------------------------------------------------------------
ActionState::Data ActionState::GetState() const
{
    return inputState_.u16;
}

// ----------------------------------------------------------------------------
float ActionState::GetLeft() const
{
    return float(inputState_.data.left) / 0x3F;
}

// ----------------------------------------------------------------------------
void ActionState::SetLeft(float value)
{
    inputState_.data.left = unsigned(value * 0x3F);  // 6 bits of range
}

// ----------------------------------------------------------------------------
float ActionState::GetRight() const
{
    return float(inputState_.data.right) / 0x3F;
}

// ----------------------------------------------------------------------------
void ActionState::SetRight(float value)
{
    inputState_.data.right = unsigned(value * 0x3F);  // 6 bits of range
}

// ----------------------------------------------------------------------------
bool ActionState::IsThrusting() const
{
    return (inputState_.data.thrust == 1);
}

// ----------------------------------------------------------------------------
void ActionState::SetThrusting(bool enable)
{
    inputState_.data.thrust = enable;
}

// ----------------------------------------------------------------------------
bool ActionState::IsFiring() const
{
    return (inputState_.data.fire == 1);
}

// ----------------------------------------------------------------------------
void ActionState::SetFiring(bool enable)
{
    inputState_.data.fire = enable;
}

// ----------------------------------------------------------------------------
void ActionState::SetWarp(bool enable)
{
    if (enable && inputState_.data.warp == false)
        SendEvent(E_ACTIONWARP, GetEventDataMap());
    inputState_.data.warp = enable;
}

// ----------------------------------------------------------------------------
void ActionState::SetUseItem(bool enable)
{
    if (enable && inputState_.data.useItem == false)
        SendEvent(E_ACTIONUSEITEM, GetEventDataMap());
    inputState_.data.useItem = enable;
}

}
