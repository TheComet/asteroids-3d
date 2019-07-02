#pragma once

#include <Urho3D/Core/Object.h>

namespace Asteroids {

URHO3D_EVENT(E_INVALIDUSERNAME, InvalidUserName)
{
}

URHO3D_EVENT(E_USERNAMETOOLONG, UsernameTooLong)
{
    URHO3D_PARAM(P_MAXLENGTH, MaxLength);  // Integer
}

URHO3D_EVENT(E_USERNAMEALREADYEXISTS, UsernameAlreadyExists)
{
}

URHO3D_EVENT(E_USERJOINED, UserJoined)
{
    URHO3D_PARAM(P_USERNAME, Username);  // String
}

URHO3D_EVENT(E_USERLEFT, UserLeft)
{
    URHO3D_PARAM(P_USERNAME, Username);  // String
}

URHO3D_EVENT(E_USERLIST, UserList)
{
    URHO3D_PARAM(P_USERS, Users);  // StringVector
}

}
