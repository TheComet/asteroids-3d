#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Asteroid.hpp"
#include "Asteroids/Bullet.hpp"
#include "Asteroids/InputActionMapper.hpp"
#include "Asteroids/OrbitingCameraController.hpp"
#include "Asteroids/Player.hpp"

#include <Urho3D/Core/Context.h>

using namespace Urho3D;

namespace Asteroids {

const char* ASTEROIDS_CATEGORY = "Asteroids";

// ----------------------------------------------------------------------------
void RegisterObjectFactories(Context* context)
{
    context->RegisterFactory<Asteroid>();
    context->RegisterFactory<Bullet>();
    context->RegisterFactory<InputActionMapper>();
    context->RegisterFactory<OrbitingCameraController>();
    context->RegisterFactory<Player>();
}

}
