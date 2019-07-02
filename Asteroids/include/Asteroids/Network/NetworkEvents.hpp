#pragma once

#include <Urho3D/Core/Object.h>

namespace Asteroids {

URHO3D_EVENT(E_SHIPUPDATED, ShipUpdated)
{
    URHO3D_PARAM(P_TIMESTAMP, Timestamp);      // uint8_t
    URHO3D_PARAM(P_ACTIONSTATE, ActionState);  // uint16_t
}

}
