#pragma once

#include <Urho3D/Core/Object.h>

namespace Asteroids {

URHO3D_EVENT(E_INVALIDUSERNAME, InvalidUserName)
{
}

URHO3D_EVENT(E_USERNAMETOOLONG, UsernameTooLong)
{
    URHO3D_PARAM(P_MAXLENGTH, MaxLength);
}

URHO3D_EVENT(E_USERNAMEALREADYEXISTS, UsernameAlreadyExists)
{
}

}
