#pragma once

#include <Urho3D/Core/Object.h>

namespace Asteroids {

/*!
 * @brief Sent when the main menu "Connect to Server" button is pressed
 */
URHO3D_EVENT(E_MAINMENUCONNECT, MainMenuConnect)
{
}

/*!
 * @brief Sent when the main menu "Host Server" button is pressed.
 */
URHO3D_EVENT(E_MAINMENUHOST, MainMenuHost)
{
}

/*!
 * @brief Sent when the main menu "Options" button is pressed.
 */
URHO3D_EVENT(E_MAINMENUOPTIONS, MainMenuOptions)
{
}

/*!
 * @brief Sent when the main menu "Quit" button is pressed
 */
URHO3D_EVENT(E_MAINMENUQUIT, MenuQuit)
{
}

/*!
 * @brief Sent when the connect prompt button "Connect" is pressed. As of now,
 * the ClientApplication initiates the connection in response to this event
 * because the scene must be passed in to Network::Connect().
 */
URHO3D_EVENT(E_CONNECTPROMPTREQUESTCONNECT, ConnectPromptRequestConnect)
{
    URHO3D_PARAM(P_USERNAME, Username);  // String
    URHO3D_PARAM(P_ADDRESS, Address);    // String
    URHO3D_PARAM(P_PORT, Port);          // int
}

/*!
 * @brief Sent when the connection prompt cancel button is pressed while a
 * connection is in progress.
 */
URHO3D_EVENT(E_CONNECTPROMPTREQUESTCANCEL, ConnectPromptRequestCancel)
{
}

/*!
 * @brief Sent when the connection to the server succeeds and the prompt should
 * be hidden.
 */
URHO3D_EVENT(E_CONNECTPROMPTSUCCESS, ConnectPromptSuccess)
{
}

/*!
 * @brief Sent when the connect prompt button "Cancel" is pressed when there
 * is no connection in progress. This should switch the menu back to the main
 * menu screen.
 */
URHO3D_EVENT(E_CONNECTPROMPTGOBACK, ConnectPromptGoBack)
{
}

/*!
 * @brief Sent when the host server prompt button "Host" is pressed. As of now,
 * the ClientApplication initiates the connection in response to this event
 * because the scene must be passed in to Network::Connect().
 */
URHO3D_EVENT(E_HOSTSERVERPROMPTREQUESTCONNECT, HostServerPromptRequestConnect)
{
    URHO3D_PARAM(P_USERNAME, Username);  // String
    URHO3D_PARAM(P_PORT, Port);          // int
}

/*!
 * @brief Sent when the host server prompt cancel button is pressed while a
 * connection is in progress.
 */
URHO3D_EVENT(E_HOSTSERVERPROMPTREQUESTCANCEL, HostServerPromptRequestCancel)
{
}

/*!
 * @brief Sent when the connection to the server succeeds and the prompt should
 * be hidden.
 */
URHO3D_EVENT(E_HOSTSERVERPROMPTSUCCESS, HostServerPromptSuccess)
{
}

/*!
 * @brief Sent when the host server prompt button "Cancel" is pressed when there
 * is no connection in progress. This should switch the menu back to the main
 * menu screen.
 */
URHO3D_EVENT(E_HOSTSERVERPROMPTGOBACK, HostServerPromptGoBack)
{
}

}
