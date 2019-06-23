#pragma once

#include "Asteroids/Config.hpp"
#include <Urho3D/Scene/Component.h>

namespace Asteroids {

/*!
 * @brief Base class for all objects that move around the surface of a planet.
 *
 *
 */
class ASTEROIDS_PUBLIC_API SurfaceObject : public Urho3D::Component
{
    URHO3D_OBJECT(SurfaceObject, Urho3D::Component)

public:
    SurfaceObject(Urho3D::Context* context);

    float GetPlanetHeight() const;

protected:
    void UpdatePosition(const Urho3D::Vector2& localLinearVelocity, float dt);
    void UpdatePlanetHeight(float offset=0.0);

private:

    float planetHeight_;
};

}