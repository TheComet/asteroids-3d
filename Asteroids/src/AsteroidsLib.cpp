#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Player/ActionState.hpp"
#include "Asteroids/Player/DeviceInputMapper.hpp"
#include "Asteroids/Player/OrbitingCameraController.hpp"
#include "Asteroids/Player/ShipController.hpp"
#include "Asteroids/Objects/Asteroid.hpp"
#include "Asteroids/Objects/Bullet.hpp"
#include "Asteroids/Network/NetworkShipState.hpp"

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

    ActionState::RegisterObject(context);
    DeviceInputMapper::RegisterObject(context);
    NetworkShipState::RegisterObject(context);
    ShipController::RegisterObject(context);
}

}
