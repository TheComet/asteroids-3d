#include "Asteroids/UserRegistry/UserRegistry.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/Connection.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
UserRegistry::UserRegistry(Context* context) :
    Object(context)
{
}

// ----------------------------------------------------------------------------
const User* UserRegistry::GetUser(Connection* connection) const
{
    // Find user with this connection
    for (ConstIterator it = users_.Begin(); it != users_.End(); ++it)
        if (it->second_.GetConnection() == connection)
            return &it->second_;

    assert(false);  // Server has a connection object that isn't registered? Should never happen
    return nullptr;
}

// ----------------------------------------------------------------------------
const User* UserRegistry::GetUser(uint32_t guid) const
{
    assert(users_.Find(guid) != users_.End());
    return &users_.Find(guid)->second_;
}

// ----------------------------------------------------------------------------
const User* UserRegistry::FindUser(const String& username) const
{
    for (const auto& user : users_)
        if (user.second_.GetUsername() == username)
            return &user.second_;
    return nullptr;
}

// ----------------------------------------------------------------------------
const UserRegistry::UsersType& UserRegistry::GetAllUsers() const
{
    return users_;
}

// ----------------------------------------------------------------------------
bool UserRegistry::IsUsernameTaken(const String& name) const
{
    for (const auto& user : users_)
        if (user.second_.GetUsername() == name)
            return true;
    return false;
}

// ----------------------------------------------------------------------------
const User* UserRegistry::AddUser(const String& name, Connection* connection)
{
    User newUser(name, connection);
    return &users_.Insert(MakePair(newUser.GetGUID(), newUser))->second_;
}

// ----------------------------------------------------------------------------
const User* UserRegistry::AddUser(const String& name, uint32_t guid)
{
    assert(users_.Find(guid) == users_.End());
    User newUser(name, guid);
    return &users_.Insert(MakePair(guid, newUser))->second_;
}

// ----------------------------------------------------------------------------
bool UserRegistry::RemoveUser(Connection* connection)
{
    // Find user with this connection
    for (Iterator it = users_.Begin(); it != users_.End(); ++it)
        if (it->second_.GetConnection() == connection)
        {
            users_.Erase(it);
            return true;
        }
    return false;
}

// ----------------------------------------------------------------------------
bool UserRegistry::RemoveUser(uint32_t guid)
{
    return users_.Erase(guid);
}

// ----------------------------------------------------------------------------
void UserRegistry::ClearAll()
{
    users_.Clear();
}

}
