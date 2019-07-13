#pragma once

#include <Urho3D/Core/Object.h>

namespace Asteroids {

// Gets sent when the server acknowledges you have joined successfully
URHO3D_EVENT(E_REGISTERSUCCEEDED, RegisterSucceeded)
{
    URHO3D_PARAM(P_GUID, Guid);            // UShort
}

URHO3D_EVENT(E_REGISTERFAILED, RegisterFailed)
{
    URHO3D_PARAM(P_REASON, Reason);        // String
}

URHO3D_EVENT(E_USERJOINED, UserJoined)
{
    URHO3D_PARAM(P_GUID, Guid);            // UShort
    URHO3D_PARAM(P_USERNAME, Username);    // String
}

URHO3D_EVENT(E_USERLEFT, UserLeft)
{
    URHO3D_PARAM(P_GUID, Guid);            // UShort
}

}
