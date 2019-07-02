#pragma once

#include "Asteroids/Config.hpp"
#include <Urho3D/Core/Object.h>

namespace Urho3D {
    class Connection;
}

namespace Asteroids {

class ASTEROIDS_PUBLIC_API User
{
public:
    User() : connection_(nullptr) {}
    User(Urho3D::String username, Urho3D::Connection* connection) :
        username_(username), connection_(connection) {}

    Urho3D::String username_;
    Urho3D::Connection* connection_;
};

}
