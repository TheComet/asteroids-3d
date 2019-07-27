#include "Asteroids/Menu/Menu.hpp"
#include "Asteroids/Menu/MenuEvents.hpp"
#include "Asteroids/Menu/MainMenu.hpp"
#include "Asteroids/Menu/ConnectPrompt.hpp"
#include "Asteroids/Menu/HostServerPrompt.hpp"

#include <Urho3D/UI/UI.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
Menu::Menu(Context* context) :
    Object(context),
    screens_{}
{
}

// ----------------------------------------------------------------------------
void Menu::StartMainMenu()
{
    UI* ui = GetSubsystem<UI>();

    if (screens_[0] != nullptr)
        return;

    screens_[MAIN_MENU] = ui->GetRoot()->CreateChild<MainMenu>();
    screens_[CONNECT_PROMPT1] = ui->GetRoot()->CreateChild<ConnectPrompt>();
    screens_[HOST_SERVER_PROMPT] = ui->GetRoot()->CreateChild<HostServerPrompt>();

    SwitchToScreen(MAIN_MENU);

    SubscribeToEvent(screens_[MAIN_MENU], E_MAINMENUCONNECT, URHO3D_HANDLER(Menu, HandleMainMenuConnect));
    SubscribeToEvent(screens_[MAIN_MENU], E_MAINMENUHOST, URHO3D_HANDLER(Menu, HandleMainMenuHost));
    SubscribeToEvent(screens_[MAIN_MENU], E_MAINMENUQUIT, URHO3D_HANDLER(Menu, HandleMainMenuQuit));

    SubscribeToEvent(screens_[CONNECT_PROMPT1], E_CONNECTPROMPTGOBACK, URHO3D_HANDLER(Menu, HandleConnectPromptCancel));
    SubscribeToEvent(screens_[CONNECT_PROMPT1], E_CONNECTPROMPTSUCCESS, URHO3D_HANDLER(Menu, HandleConnectPromptSuccess));

    SubscribeToEvent(screens_[HOST_SERVER_PROMPT], E_HOSTSERVERPROMPTGOBACK, URHO3D_HANDLER(Menu, HandleHostServerPromptCancel));
    SubscribeToEvent(screens_[HOST_SERVER_PROMPT], E_HOSTSERVERPROMPTSUCCESS, URHO3D_HANDLER(Menu, HandleHostServerPromptSuccess));
}

// ----------------------------------------------------------------------------
void Menu::StartPauseMenu()
{
}

// ----------------------------------------------------------------------------
void Menu::SwitchToScreen(Screen screen)
{
    HideAllScreens();
    screens_[screen]->Show();
}

// ----------------------------------------------------------------------------
void Menu::HideAllScreens()
{
    for (int i = 0; i != SCREEN_COUNT; ++i)
        screens_[i]->Hide();
}

// ----------------------------------------------------------------------------
void Menu::HandleMainMenuConnect(StringHash eventType, VariantMap& eventData)
{
    SwitchToScreen(CONNECT_PROMPT1);
}

// ----------------------------------------------------------------------------
void Menu::HandleMainMenuHost(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    SwitchToScreen(HOST_SERVER_PROMPT);
}

// ----------------------------------------------------------------------------
void Menu::HandleMainMenuQuit(StringHash eventType, VariantMap& eventData)
{
    HideAllScreens();
}

// ----------------------------------------------------------------------------
void Menu::HandleConnectPromptCancel(StringHash eventType, VariantMap& eventData)
{
    SwitchToScreen(MAIN_MENU);
}

// ----------------------------------------------------------------------------
void Menu::HandleConnectPromptSuccess(StringHash eventType, VariantMap& eventData)
{
    HideAllScreens();
}

// ----------------------------------------------------------------------------
void Menu::HandleHostServerPromptCancel(StringHash eventType, VariantMap& eventData)
{
    SwitchToScreen(MAIN_MENU);
}

// ----------------------------------------------------------------------------
void Menu::HandleHostServerPromptSuccess(StringHash eventType, VariantMap& eventData)
{
    HideAllScreens();
}

}
