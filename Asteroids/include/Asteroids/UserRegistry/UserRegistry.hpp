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

    typedef Urho3D::HashMap<User::GUID, Urho3D::SharedPtr<User>> UsersType;
    typedef UsersType::Iterator Iterator;
    typedef UsersType::ConstIterator ConstIterator;

public:
    UserRegistry(Urho3D::Context* context);

    User* GetUser(Urho3D::Connection* connection) const;
    User* GetUser(User::GUID guid) const;
    User* FindUser(const Urho3D::String& name) const;
    const UsersType& GetAllUsers() const;

private:
    friend class ServerUserRegistry;
    friend class ClientUserRegistry;

    bool IsUsernameTaken(const Urho3D::String& name) const;
    User* AddUser(const Urho3D::String& name, User::GUID guid);
    User* AddUser(const Urho3D::String& name, Urho3D::Connection* connection);
    Urho3D::SharedPtr<User> RemoveUser(Urho3D::Connection* connection);
    Urho3D::SharedPtr<User> RemoveUser(User::GUID guid);
    void ClearAll();

private:
    UsersType users_;
};

}
