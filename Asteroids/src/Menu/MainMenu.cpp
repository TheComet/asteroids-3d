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
    Button* options = GetUIChild<Button>(ui, "Button_Options");
    Button* quit = GetUIChild<Button>(ui, "Button_Quit");

    if (connect)
        SubscribeToEvent(connect, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleConnect));
    else
        URHO3D_LOGERROR("Failed to get button Button_Connect");

    if (options)
        SubscribeToEvent(options, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleOptions));
    else
        URHO3D_LOGERROR("Failed to get button Button_Connect");

    if (quit)
        SubscribeToEvent(quit, E_RELEASED, URHO3D_HANDLER(MainMenu, HandleQuit));
    else
        URHO3D_LOGERROR("Failed to get button Button_Connect");
}

// ----------------------------------------------------------------------------
void MainMenu::HandleConnect(StringHash eventType, VariantMap& eventData)
{
    SendEvent(E_MAINMENUCONNECT);
}

// ----------------------------------------------------------------------------
void MainMenu::HandleOptions(StringHash eventType, VariantMap& eventData)
{

}

// ----------------------------------------------------------------------------
void MainMenu::HandleQuit(StringHash eventType, VariantMap& eventData)
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

    if (eventData[P_KEY].GetInt() == KEY_ESCAPE)
        SendEvent(E_MAINMENUQUIT);
}

}
