#include "Asteroids/Globals.hpp"
#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Menu/HostServerPrompt.hpp"
#include "Asteroids/Menu/MenuEvents.hpp"
#include "Asteroids/Util/UIUtils.hpp"
#include "Asteroids/UserRegistry/UserRegistryEvents.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Resource/XMLElement.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UIEvents.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
HostServerPrompt::HostServerPrompt(Context* context) :
    MenuScreen(context),
    info_(nullptr),
    username_(nullptr),
    port_(nullptr),
    cancel_(nullptr),
    host_(nullptr),
    connectionInProgress_(false)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    configFile_ = cache->GetResource<XMLFile>("UI/HostServerPrompt.xml");
    if (configFile_ == nullptr)
        return;

    SetAlignment(HA_CENTER, VA_CENTER);
    LoadXMLAndInit();
}

// ----------------------------------------------------------------------------
void HostServerPrompt::RegisterObject(Context* context)
{
    context->RegisterFactory<HostServerPrompt>(ASTEROIDS_CATEGORY);
}

// ----------------------------------------------------------------------------
void HostServerPrompt::Show()
{
    SetVisible(true);
    info_->SetColor(Color::WHITE);
    info_->SetText("Host a Server:");
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(HostServerPrompt, HandleKeyDown));
}

// ----------------------------------------------------------------------------
void HostServerPrompt::Hide()
{
    SetVisible(false);
    UnsubscribeFromEvent(E_KEYDOWN);
}

// ----------------------------------------------------------------------------
void HostServerPrompt::LoadXMLAndInit()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    RemoveAllChildren();
    UIElement* ui = LoadChildXML(configFile_->GetRoot(), cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

    info_ = GetUIChild<Text>(ui, "Text_Info");
    username_ = GetUIChild<LineEdit>(ui, "LineEdit_Username");
    port_ = GetUIChild<LineEdit>(ui, "LineEdit_Port");
    cancel_ = GetUIChild<Button>(ui, "Button_Cancel");
    host_ = GetUIChild<Button>(ui, "Button_Host");

    if (info_ == nullptr)
        URHO3D_LOGERROR("Failed to get UI element Text_Info");

    if (username_)
        SubscribeToEvent(username_, E_TEXTCHANGED, URHO3D_HANDLER(HostServerPrompt, HandleTextChanged));
    else
        URHO3D_LOGERROR("Failed to get UI element LineEdit_Username");

    if (port_)
        SubscribeToEvent(port_, E_TEXTCHANGED, URHO3D_HANDLER(HostServerPrompt, HandleTextChanged));
    else
        URHO3D_LOGERROR("Failed to get UI element LineEdit_Port");

    if (cancel_)
        SubscribeToEvent(cancel_, E_RELEASED, URHO3D_HANDLER(HostServerPrompt, HandleButtonCancel));
    else
        URHO3D_LOGERROR("Failed to get UI element Button_Cancel");

    if (host_)
        SubscribeToEvent(host_, E_RELEASED, URHO3D_HANDLER(HostServerPrompt, HandleButtonHost));
    else
        URHO3D_LOGERROR("Failed to get UI element Button_Host");

    LoadSettings();
}

// ----------------------------------------------------------------------------
void HostServerPrompt::SaveSettings()
{
    // Save the previously typed in values to XML so we don't have to type in
    // username/ip every time
    FileSystem* fs = GetSubsystem<FileSystem>();
    String url = fs->GetAppPreferencesDir("TheComet", "Asteroids");
    url.Append("HostServerPromptSettings.xml");

    XMLFile xml(context_);
    XMLElement settings = xml.CreateRoot("settings");
    settings.CreateChild("username").SetValue(username_->GetText());
    settings.CreateChild("port").SetValue(port_->GetText());

    File f(context_);
    if (f.Open(url, FILE_WRITE) == false)
        return;
    URHO3D_LOGDEBUGF("Saving host server prompt settings to %s", url.CString());
    xml.Save(f);
}

// ----------------------------------------------------------------------------
void HostServerPrompt::LoadSettings()
{
    FileSystem* fs = GetSubsystem<FileSystem>();
    String url = fs->GetAppPreferencesDir("TheComet", "Asteroids");
    url.Append("HostServerPromptSettings.xml");

    File f(context_);
    if (f.Open(url, FILE_READ) == false)
        return;
    XMLFile xml(context_);
    xml.Load(f);

    XMLElement settings = xml.GetRoot();
    username_->SetText(settings.GetChild("username").GetValue());
    String port = settings.GetChild("port").GetValue();
    port_->SetText(port.Length() > 0 ? port : String(DEFAULT_PORT));
}

// ----------------------------------------------------------------------------
void HostServerPrompt::InitiateConnectionProcess()
{
    // Urho3D doesn't like multiple pending Network::Connect() calls. Disable
    // the connect button to prevent this
    if (connectionInProgress_)
        return;

    if (port_->GetText().Length() == 0)
        port_->SetText(String(DEFAULT_PORT));

    SaveSettings();
    LockInput();
    SubscribeToRegistryEvents();
    connectionInProgress_ = true;

    info_->SetText("Starting Server...");
    info_->SetColor(Color::WHITE);

    // The Network::Connect() call needs to be given a pointer to the scene,
    // but we aren't responsible for managing the scene. Therefore, send an
    // event requesting to be connected so another system can do it for us.
    using namespace HostServerPromptRequestConnect;
    VariantMap& eventData = GetEventDataMap();
    eventData[P_USERNAME] = username_->GetText();
    eventData[P_PORT] = ToUInt(port_->GetText());
    eventData[P_SUCCESS] = true;
    SendEvent(E_HOSTSERVERPROMPTREQUESTCONNECT, eventData);

    if (eventData[P_SUCCESS].GetBool() == false)
    {
        UnlockInput();
        UnsubscribeFromRegistryEvents();
        connectionInProgress_ = false;
        info_->SetColor(Color::RED);
        info_->SetText("Failed to start Server");
    }
}

// ----------------------------------------------------------------------------
void HostServerPrompt::CancelConnectionProcess()
{
    // If a connection is not in progress, then the cancel button acts as a
    // "go back to previous screen" button. Otherwise it aborts the connection
    // in progress
    if (connectionInProgress_ == false)
    {
        SendEvent(E_HOSTSERVERPROMPTGOBACK);
        return;
    }

    info_->SetColor(Color::WHITE);
    info_->SetText("Connection canceled");

    UnlockInput();
    UnsubscribeFromRegistryEvents();
    connectionInProgress_ = false;
    SendEvent(E_HOSTSERVERPROMPTREQUESTCANCEL);
}

// ----------------------------------------------------------------------------
void HostServerPrompt::LockInput()
{
    host_->SetEnabled(false);
    username_->SetEnabled(false);
    port_->SetEnabled(false);
}

// ----------------------------------------------------------------------------
void HostServerPrompt::UnlockInput()
{
    host_->SetEnabled(true);
    username_->SetEnabled(true);
    port_->SetEnabled(true);
}

// ----------------------------------------------------------------------------
void HostServerPrompt::HandleButtonCancel(StringHash eventType, VariantMap& eventData)
{
    CancelConnectionProcess();
}

// ----------------------------------------------------------------------------
void HostServerPrompt::HandleButtonHost(StringHash eventType, VariantMap& eventData)
{
    InitiateConnectionProcess();
}

// ----------------------------------------------------------------------------
void HostServerPrompt::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyDown;

    switch (eventData[P_KEY].GetInt())
    {
        case KEY_ESCAPE  : CancelConnectionProcess(); break;
        case KEY_RETURN  :
        case KEY_RETURN2 : InitiateConnectionProcess(); break;
    }
}

// ----------------------------------------------------------------------------
void HostServerPrompt::HandleTextChanged(StringHash eventType, VariantMap& eventData)
{
    // Only enable the connect button if the username text field contains text
    if (username_->GetText().Length() == 0)
        host_->SetEnabled(false);
    else
        host_->SetEnabled(true);
}

// ----------------------------------------------------------------------------
void HostServerPrompt::SubscribeToRegistryEvents()
{
    SubscribeToEvent(E_REGISTERFAILED, URHO3D_HANDLER(HostServerPrompt, HandleRegisterFailed));
    SubscribeToEvent(E_REGISTERSUCCEEDED, URHO3D_HANDLER(HostServerPrompt, HandleRegisterSucceeded));
}

// ----------------------------------------------------------------------------
void HostServerPrompt::UnsubscribeFromRegistryEvents()
{
    UnsubscribeFromEvent(E_REGISTERFAILED);
    UnsubscribeFromEvent(E_REGISTERSUCCEEDED);
}

// ----------------------------------------------------------------------------
void HostServerPrompt::HandleRegisterFailed(StringHash eventType, VariantMap& eventData)
{
    CancelConnectionProcess();
    info_->SetText("Error: " + eventData[RegisterFailed::P_REASON].GetString());
    info_->SetColor(Color::RED);
}

// ----------------------------------------------------------------------------
void HostServerPrompt::HandleRegisterSucceeded(StringHash eventType, VariantMap& eventData)
{
    UnsubscribeFromRegistryEvents();
    SendEvent(E_HOSTSERVERPROMPTSUCCESS);
}

}
