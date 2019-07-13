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
    User();
    User(const Urho3D::String& username, Urho3D::Connection* connection);
    User(const Urho3D::String& username, uint32_t guid);

    const Urho3D::String& GetUsername() const;
    Urho3D::Connection* GetConnection() const;
    uint32_t GetGUID() const;

private:
    Urho3D::String username_;
    union {
        Urho3D::Connection* connection_;
        uint32_t guid_;
    };
};

}
