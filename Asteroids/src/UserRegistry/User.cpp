#include "Asteroids/UserRegistry/User.hpp"

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
User::User() :
    connection_(nullptr)
{
}

// ----------------------------------------------------------------------------
User::User(const String& username, Connection* connection) :
    username_(username),
    connection_(connection)
{
}

// ----------------------------------------------------------------------------
User::User(const String& username, uint32_t guid) :
    username_(username),
    guid_(guid)
{
}

// ----------------------------------------------------------------------------
const String& User::GetUsername() const
{
    return username_;
}

// ----------------------------------------------------------------------------
Connection* User::GetConnection() const
{
    return connection_;
}

// ----------------------------------------------------------------------------
uint32_t User::GetGUID() const
{
    return guid_;
}

}
