#pragma once

#include "Asteroids/Config.hpp"

namespace Urho3D {
    class Context;
}

namespace Asteroids {

/*! @brief Category string objects are registered under. Needed by editor */
ASTEROIDS_PUBLIC_API extern const char* ASTEROIDS_CATEGORY;

/*!
 * @brief Registers all Urho3D objects to the specified context.
 *
 * Applications must call this before being able to instantiate Asteroids
 * objects.
 */
ASTEROIDS_PUBLIC_API void RegisterObjectFactories(Urho3D::Context* context);

}
