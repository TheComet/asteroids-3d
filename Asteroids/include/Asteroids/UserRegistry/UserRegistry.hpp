#pragma once

#include "Asteroids/Config.hpp"
#include "Asteroids/UserRegistry/User.hpp"

#include <Urho3D/Core/Object.h>
#include <Urho3D/Container/HashMap.h>

namespace Urho3D {
    class Connection;
}

namespace Asteroids {

class ServerUserRegistry;
class ClientUserRegistry;

class ASTEROIDS_PUBLIC_API UserRegistry : public Urho3D::Object
{
    URHO3D_OBJECT(UserRegistry, Urho3D::Object)

    typedef Urho3D::HashMap<Urho3D::String, User> UsersType;
    typedef UsersType::Iterator Iterator;
    typedef UsersType::ConstIterator ConstIterator;

public:
    UserRegistry(Urho3D::Context* context);

    const User& GetUser(const Urho3D::String& name) const;
    const UsersType& GetUsers() const;

private:
    friend class ServerUserRegistry;
    friend class ClientUserRegistry;
    bool AddUser(const Urho3D::String& name, Urho3D::Connection* connection);
    bool RemoveUser(Urho3D::Connection* connection);
    bool RemoveUser(const Urho3D::String& name);
    void ClearAll();

private:
    UsersType users_;
};

}
