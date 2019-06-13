#pragma once

#include <Urho3D/Core/Object.h>

namespace Asteroids {

class Player : public Urho3D::Object
{
    URHO3D_OBJECT(Player, Urho3D::Object)

public:
    Player(Urho3D::Context* context);

    static void RegisterObject(Urho3D::Context* context);

};

}
