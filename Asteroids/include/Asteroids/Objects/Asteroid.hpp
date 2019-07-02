#pragma once

#include "Asteroids/Config.hpp"
#include "Asteroids/Objects/SurfaceObject.hpp"

namespace Asteroids {

class ASTEROIDS_PUBLIC_API Asteroid : public SurfaceObject
{
    URHO3D_OBJECT(Asteroid, SurfaceObject)

public:
    Asteroid(Urho3D::Context* context);
};

}
