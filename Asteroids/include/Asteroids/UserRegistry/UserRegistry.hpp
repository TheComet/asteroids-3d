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

    typedef Urho3D::HashMap<uint32_t, User> UsersType;
    typedef UsersType::Iterator Iterator;
    typedef UsersType::ConstIterator ConstIterator;

public:
    UserRegistry(Urho3D::Context* context);

    const User* GetUser(Urho3D::Connection* connection) const;
    const User* GetUser(uint32_t guid) const;
    const User* FindUser(const Urho3D::String& name) const;
    const UsersType& GetAllUsers() const;

private:
    friend class ServerUserRegistry;
    friend class ClientUserRegistry;

    bool IsUsernameTaken(const Urho3D::String& name) const;
    const User* AddUser(const Urho3D::String& name, uint32_t guid);
    const User* AddUser(const Urho3D::String& name, Urho3D::Connection* connection);
    bool RemoveUser(Urho3D::Connection* connection);
    bool RemoveUser(uint32_t guid);
    void ClearAll();

private:
    UsersType users_;
};

}
