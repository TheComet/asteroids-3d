#pragma once

#include <Urho3D/Core/Object.h>

namespace Asteroids {

URHO3D_EVENT(E_MAINMENUCONNECT, MenuConnect)
{
}

URHO3D_EVENT(E_MAINMENUQUIT, MenuQuit)
{
}

URHO3D_EVENT(E_CONNECTPROMPTREQUESTCANCEL, ConnectPromptCancel)
{
}

URHO3D_EVENT(E_CONNECTPROMPTREQUESTCONNECT, ConnectPromptRequestConnect)
{
    URHO3D_PARAM(P_USERNAME, Username);  // String
    URHO3D_PARAM(P_ADDRESS, Address);    // String
    URHO3D_PARAM(P_PORT, Port);          // int
}

URHO3D_EVENT(E_CONNECTPROMPTSUCCESS, ConnectPromptSuccess)
{
}

}
