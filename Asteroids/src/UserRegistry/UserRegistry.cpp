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
User* UserRegistry::GetUser(Connection* connection) const
{
    // Find user with this connection
    for (ConstIterator it = users_.Begin(); it != users_.End(); ++it)
        if (it->second_->GetConnection() == connection)
            return it->second_;

    assert(false);  // Server has a connection object that isn't registered? Should never happen
    return nullptr;
}

// ----------------------------------------------------------------------------
User* UserRegistry::GetUser(User::GUID guid) const
{
    assert(users_.Find(guid) != users_.End());
    return users_.Find(guid)->second_;
}

// ----------------------------------------------------------------------------
User* UserRegistry::FindUser(const String& username) const
{
    for (const auto& user : users_)
        if (user.second_->GetUsername() == username)
            return user.second_;
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
        if (user.second_->GetUsername() == name)
            return true;
    return false;
}

// ----------------------------------------------------------------------------
User* UserRegistry::AddUser(const String& name, Connection* connection)
{
    User* user = new User(name, connection);
    users_[user->GetGUID()] = user;
    return user;
}

// ----------------------------------------------------------------------------
User* UserRegistry::AddUser(const String& name, User::GUID guid)
{
    assert(users_.Find(guid) == users_.End());
    User* user = new User(name, guid);
    users_[guid] = user;
    return user;
}

// ----------------------------------------------------------------------------
SharedPtr<User> UserRegistry::RemoveUser(Connection* connection)
{
    // Find user with this connection
    for (Iterator it = users_.Begin(); it != users_.End(); ++it)
        if (it->second_->GetConnection() == connection)
        {
            SharedPtr<User> user = it->second_;
            users_.Erase(it);
            return user;
        }
    return nullptr;
}

// ----------------------------------------------------------------------------
SharedPtr<User> UserRegistry::RemoveUser(User::GUID guid)
{
    Iterator it = users_.Find(guid);
    if (it != users_.End())
    {
        SharedPtr<User> user = it->second_;
        users_.Erase(it);
        return user;
    }
    return nullptr;
}

// ----------------------------------------------------------------------------
void UserRegistry::ClearAll()
{
    users_.Clear();
}

}
