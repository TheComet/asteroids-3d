#pragma once

#include <Urho3D/Core/Object.h>

namespace Asteroids {

URHO3D_EVENT(E_PLAYERCREATE, PlayerCreate)
{
    URHO3D_PARAM(P_GUID, GUID);                 // UShort: ID of user
    URHO3D_PARAM(P_PIVOTROTATION, PivotRotation);   // Quaternion: Player spawn coordinates as rotation of the pivot node
}

URHO3D_EVENT(E_PLAYERDESTROY, PlayerDestroy)
{
    URHO3D_PARAM(P_GUID, GUID);                 // UShort: ID of the user to destroy
}

}
