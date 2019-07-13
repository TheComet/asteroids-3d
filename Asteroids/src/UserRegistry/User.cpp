#include "Asteroids/UserRegistry/User.hpp"

using namespace Urho3D;

namespace Asteroids {

static uint32_t guidCounter = 0;

// ----------------------------------------------------------------------------
uint32_t GeneratePlayerGUID()
{
    if (++guidCounter > 0x7FFFFFFF)
        guidCounter = 0;
    return guidCounter;
}

// ----------------------------------------------------------------------------
uint32_t GenerateNonPlayerGUID()
{
    return GeneratePlayerGUID() | 0x80000000;
}

// ----------------------------------------------------------------------------
User::User() :
    connection_(nullptr),
    guid_(static_cast<uint32_t>(-1))
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
User::User(const Urho3D::String& username, uint32_t guid) :
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
uint32_t User::GetGUID() const
{
    return guid_;
}

// ----------------------------------------------------------------------------
bool User::IsPlayerControlled() const
{
    return (guid_ & 0x80000000) != 0;
}

}
