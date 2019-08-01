#pragma once

#include "Asteroids/Config.hpp"

namespace Urho3D {
    class Context;
    class RenderPath;
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

/*!
 * @brief Registers all network events that are sent remotely.
 *
 * Applications must call this before being able to communicate over the
 * network.
 */
ASTEROIDS_PUBLIC_API void RegisterRemoteNetworkEvents(Urho3D::Context* context);

/*!
 * @brief Loads the custom renderpath and returns it.
 */
ASTEROIDS_PUBLIC_API Urho3D::RenderPath* LoadRenderPath(Urho3D::Context* context);

}
