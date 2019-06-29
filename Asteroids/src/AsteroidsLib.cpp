#include "Asteroids/ActionState.hpp"
#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Asteroid.hpp"
#include "Asteroids/Bullet.hpp"
#include "Asteroids/DeviceInputMapper.hpp"
#include "Asteroids/OrbitingCameraController.hpp"
#include "Asteroids/ShipController.hpp"

#include <Urho3D/Core/Context.h>

using namespace Urho3D;

namespace Asteroids {

const char* ASTEROIDS_CATEGORY = "Asteroids";

// ----------------------------------------------------------------------------
void RegisterObjectFactories(Context* context)
{
    context->RegisterFactory<Asteroid>(ASTEROIDS_CATEGORY);
    context->RegisterFactory<Bullet>(ASTEROIDS_CATEGORY);
    context->RegisterFactory<OrbitingCameraController>(ASTEROIDS_CATEGORY);

    DeviceInputMapper::RegisterObject(context);
    ActionState::RegisterObject(context);
    ShipController::RegisterObject(context);
}

}
