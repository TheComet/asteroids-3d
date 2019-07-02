#include "Asteroids/UserRegistry/UserRegistry.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Network/Connection.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
UserRegistry::UserRegistry(Context* context) :
    Object(context)
{
}

// ----------------------------------------------------------------------------
bool UserRegistry::AddUser(const String& name, Connection* connection)
{
    Iterator existing = users_.Find(name);
    if (existing != users_.End())
        return false;
    users_[name] = User(name, connection);

    return true;
}

// ----------------------------------------------------------------------------
bool UserRegistry::RemoveUser(const String& name)
{
    return users_.Erase(name);
}

// ----------------------------------------------------------------------------
bool UserRegistry::RemoveUser(Connection* connection)
{
    for (Iterator it = users_.Begin(); it != users_.End(); ++it)
        if (it->second_.connection_ == connection)
        {
            users_.Erase(it);
            return true;
        }
    return false;
}

// ----------------------------------------------------------------------------
void UserRegistry::ClearAll()
{
    users_.Clear();
}

// ----------------------------------------------------------------------------
const User& UserRegistry::GetUser(const String& username) const
{
    return users_.Find(username)->second_;
}

// ----------------------------------------------------------------------------
const UserRegistry::UsersType& UserRegistry::GetUsers() const
{
    return users_;
}

}
