#pragma once

#include "Asteroids/Config.hpp"

namespace Urho3D {
    class Context;
}

namespace Asteroids {

ASTEROIDS_PUBLIC_API extern const char* ASTEROIDS_CATEGORY;

ASTEROIDS_PUBLIC_API void RegisterObjectFactories(Urho3D::Context* context);

}
