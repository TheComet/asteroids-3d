#include "Asteroids/Menu/Menu.hpp"
#include "Asteroids/Menu/MenuEvents.hpp"
#include "Asteroids/Menu/MainMenu.hpp"
#include "Asteroids/Menu/ConnectPrompt.hpp"

#include <Urho3D/UI/UI.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
Menu::Menu(Context* context) :
    Object(context),
    mainMenu_(nullptr),
    connectPrompt_(nullptr)
{
}

// ----------------------------------------------------------------------------
void Menu::StartMainMenu()
{
    UI* ui = GetSubsystem<UI>();

    if (mainMenu_)
        return;

    mainMenu_ = ui->GetRoot()->CreateChild<MainMenu>();
    connectPrompt_ = ui->GetRoot()->CreateChild<ConnectPrompt>();

    mainMenu_->SetVisible(true);
    connectPrompt_->SetVisible(false);

    SubscribeToEvent(mainMenu_, E_MAINMENUCONNECT, URHO3D_HANDLER(Menu, HandleMainMenuConnect));
    SubscribeToEvent(mainMenu_, E_MAINMENUQUIT, URHO3D_HANDLER(Menu, HandleMainMenuQuit));
    SubscribeToEvent(connectPrompt_, E_CONNECTPROMPTCANCEL, URHO3D_HANDLER(Menu, HandleConnectPromptCancel));
    SubscribeToEvent(connectPrompt_, E_CONNECTPROMPTSUCCESS, URHO3D_HANDLER(Menu, HandleConnectPromptSuccess));
}

// ----------------------------------------------------------------------------
void Menu::StartPauseMenu()
{
}

// ----------------------------------------------------------------------------
void Menu::HandleMainMenuConnect(StringHash eventType, VariantMap& eventData)
{
    mainMenu_->Hide();
    connectPrompt_->Show();
}

// ----------------------------------------------------------------------------
void Menu::HandleMainMenuQuit(StringHash eventType, VariantMap& eventData)
{
    mainMenu_->Hide();
}

// ----------------------------------------------------------------------------
void Menu::HandleConnectPromptCancel(StringHash eventType, VariantMap& eventData)
{
    connectPrompt_->Hide();
    mainMenu_->Show();
}

// ----------------------------------------------------------------------------
void Menu::HandleConnectPromptSuccess(StringHash eventType, VariantMap& eventData)
{
    connectPrompt_->Hide();
}

}
