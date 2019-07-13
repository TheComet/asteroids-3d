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
    assert(users_.Find(ToU32(connection)) != users_.End());
    return &users_.Find(ToU32(connection))->second_;
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
    assert(users_.Find(ToU32(connection)) == users_.End());
    return &users_.Insert(MakePair(ToU32(connection), User(name, connection)))->second_;
}

// ----------------------------------------------------------------------------
const User* UserRegistry::AddUser(const String& name, uint32_t guid)
{
    assert(users_.Find(guid) == users_.End());
    return &users_.Insert(MakePair(guid, User(name, guid)))->second_;
}

// ----------------------------------------------------------------------------
bool UserRegistry::RemoveUser(Connection* connection)
{
    return users_.Erase(ToU32(connection));
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
