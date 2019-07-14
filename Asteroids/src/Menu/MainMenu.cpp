#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Menu/MainMenu.hpp"
#include "Asteroids/Menu/MenuEvents.hpp"
#include "Asteroids/Util/UIUtils.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/ResourceEvents.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
MainMenu::MainMenu(Context* context) :
    MenuScreen(context)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    xmlFile_ = cache->GetResource<XMLFile>("UI/MainMenu.xml");
    if (xmlFile_ == nullptr)
        return;

    SetAlignment(HA_CENTER, VA_CENTER);
    LoadXMLAndInit();

    SubscribeToEvent(E_FILECHANGED, URHO3D_HANDLER(MainMenu, HandleFileChanged));
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(MainMenu, HandleKeyDown));
}

// ----------------------------------------------------------------------------
void MainMenu::RegisterObject(Context* context)
{
    context->RegisterFactory<MainMenu>(ASTEROIDS_CATEGORY);
}

// ----------------------------------------------------------------------------
void MainMenu::Show()
{
    SetVisible(true);
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(MainMenu, HandleKeyDown));
}

// ----------------------------------------------------------------------------
void MainMenu::Hide()
{
    SetVisible(false);
    UnsubscribeFromEvent(E_KEYDOWN);
}

// ----------------------------------------------------------------------------
void MainMenu::LoadXMLAndInit()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    RemoveAllChildren();
    UIElement* ui = LoadChildXML(xmlFile_->GetRoot(), cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

    Button* connect = GetUIChild<Button>(ui, "Button_Connect");
    Button* host = GetUIChild<Button>(ui, "Button_Host");
    Button* options = GetUIChild<Button>(ui, "Button_Options");
    Button* quit = GetUIChild<Button>(ui, "Button_Quit");

    if (connect)
        SubscribeToEvent(connect, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleButtonConnectToServer));
    else
        URHO3D_LOGERROR("Failed to get button Button_Connect");

    if (host)
        SubscribeToEvent(host, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleButtonHost));
    else
        URHO3D_LOGERROR("Failed to get button Button_Host");

    if (options)
        SubscribeToEvent(options, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleButtonOptions));
    else
        URHO3D_LOGERROR("Failed to get button Button_Connect");

    if (quit)
        SubscribeToEvent(quit, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleButtonQuit));
    else
        URHO3D_LOGERROR("Failed to get button Button_Connect");
}

// ----------------------------------------------------------------------------
void MainMenu::HandleButtonConnectToServer(StringHash eventType, VariantMap& eventData)
{
    SendEvent(E_MAINMENUCONNECT);
}

// ----------------------------------------------------------------------------
void MainMenu::HandleButtonHost(StringHash eventType, VariantMap& eventData)
{
    SendEvent(E_MAINMENUHOST);
    URHO3D_LOGERROR("Not implemented!");
}

// ----------------------------------------------------------------------------
void MainMenu::HandleButtonOptions(StringHash eventType, VariantMap& eventData)
{
    URHO3D_LOGERROR("Not implemented!");
}

// ----------------------------------------------------------------------------
void MainMenu::HandleButtonQuit(StringHash eventType, VariantMap& eventData)
{
    SendEvent(E_MAINMENUQUIT);
}

// ----------------------------------------------------------------------------
void MainMenu::HandleFileChanged(StringHash eventType, VariantMap& eventData)
{
    if (eventData[FileChanged::P_RESOURCENAME] == xmlFile_->GetName())
        LoadXMLAndInit();
}

// ----------------------------------------------------------------------------
void MainMenu::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyDown;

    switch (eventData[P_KEY].GetInt())
    {
        case KEY_RETURN  :
        case KEY_RETURN2 : SendEvent(E_MAINMENUCONNECT); break;
        case KEY_ESCAPE  : SendEvent(E_MAINMENUQUIT); break;
    }
}

}
