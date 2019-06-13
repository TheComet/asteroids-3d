#include "Asteroids/Player.hpp"

#include <Urho3D/Core/Context.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
Player::Player(Context* context) :
    Object(context)
{
}

// ----------------------------------------------------------------------------
void Player::RegisterObject(Urho3D::Context* context)
{
    context->RegisterFactory<Player>("Asteroids");
}

}
