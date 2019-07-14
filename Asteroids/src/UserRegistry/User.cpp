#include "Asteroids/UserRegistry/User.hpp"

using namespace Urho3D;

namespace Asteroids {

static User::GUID guidCounter = 5;
static const User::GUID MAX_PLAYER_GUID = 0x7FFF;
static const User::GUID MSB_BIT = 0x8000;

// ----------------------------------------------------------------------------
User::GUID GeneratePlayerGUID()
{
    if (++guidCounter > MAX_PLAYER_GUID)
        guidCounter = 0;
    return guidCounter;
}

// ----------------------------------------------------------------------------
User::GUID GenerateNonPlayerGUID()
{
    return GeneratePlayerGUID() | MSB_BIT;
}

// ----------------------------------------------------------------------------
User::User() :
    connection_(nullptr),
    guid_(static_cast<GUID>(-1))
{
}

// ----------------------------------------------------------------------------
User::User(const String& username, Connection* connection) :
    username_(username),
    connection_(connection),
    guid_(GeneratePlayerGUID())
{
}

// ----------------------------------------------------------------------------
User::User(const String& username) :
    username_(username),
    guid_(GenerateNonPlayerGUID())
{
}

// ----------------------------------------------------------------------------
User::User(const Urho3D::String& username, GUID guid) :
    username_(username),
    connection_(nullptr),
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
User::GUID User::GetGUID() const
{
    return guid_;
}

// ----------------------------------------------------------------------------
bool User::IsPlayerControlled() const
{
    return (guid_ & MSB_BIT) != 0;
}

}
