#include "Asteroids/Globals.hpp"
#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Menu/ConnectPrompt.hpp"
#include "Asteroids/Menu/MenuEvents.hpp"
#include "Asteroids/UserRegistry/UserRegistryEvents.hpp"
#include "Asteroids/UserRegistry/ClientUserRegistry.hpp"
#include "Asteroids/Util/UIUtils.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/ResourceEvents.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Text.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
ConnectPrompt::ConnectPrompt(Context* context) :
    MenuScreen(context),
    info_(nullptr),
    username_(nullptr),
    ipAddress_(nullptr),
    port_(nullptr),
    cancel_(nullptr),
    connect_(nullptr)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    xmlFile_ = cache->GetResource<XMLFile>("UI/ConnectPrompt.xml");
    if (xmlFile_ == nullptr)
        return;

    SetAlignment(HA_CENTER, VA_CENTER);
    LoadXMLAndInit();
}

// ----------------------------------------------------------------------------
void ConnectPrompt::RegisterObject(Context* context)
{
    context->RegisterFactory<ConnectPrompt>(ASTEROIDS_CATEGORY);
}

// ----------------------------------------------------------------------------
void ConnectPrompt::Show()
{
    SetVisible(true);
    info_->SetColor(Color::WHITE);
    info_->SetText("Connect to a server:");
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(ConnectPrompt, HandleKeyDown));
}

// ----------------------------------------------------------------------------
void ConnectPrompt::Hide()
{
    SetVisible(false);
    UnsubscribeFromEvent(E_KEYDOWN);
}

// ----------------------------------------------------------------------------
void ConnectPrompt::LoadXMLAndInit()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    RemoveAllChildren();
    UIElement* ui = LoadChildXML(xmlFile_->GetRoot(), cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));

    info_ = GetUIChild<Text>(ui, "Text_Info");
    username_ = GetUIChild<LineEdit>(ui, "LineEdit_Username");
    ipAddress_ = GetUIChild<LineEdit>(ui, "LineEdit_IPAddress");
    port_ = GetUIChild<LineEdit>(ui, "LineEdit_Port");
    cancel_ = GetUIChild<Button>(ui, "Button_Cancel");
    connect_ = GetUIChild<Button>(ui, "Button_Connect");

    if (info_ == nullptr)
        URHO3D_LOGERROR("Failed to get UI element Text_Info");

    if (username_)
        SubscribeToEvent(username_, E_TEXTCHANGED, URHO3D_HANDLER(ConnectPrompt, HandleTextChanged));
    else
        URHO3D_LOGERROR("Failed to get UI element LineEdit_Username");

    if (ipAddress_)
        SubscribeToEvent(ipAddress_, E_TEXTCHANGED, URHO3D_HANDLER(ConnectPrompt, HandleTextChanged));
    else
        URHO3D_LOGERROR("Failed to get UI element LineEdit_IPAddress");

    if (port_)
        SubscribeToEvent(port_, E_TEXTCHANGED, URHO3D_HANDLER(ConnectPrompt, HandleTextChanged));
    else
        URHO3D_LOGERROR("Failed to get UI element LineEdit_Port");

    if (cancel_)
        SubscribeToEvent(cancel_, E_RELEASED, URHO3D_HANDLER(ConnectPrompt, HandleButtonCancel));
    else
        URHO3D_LOGERROR("Failed to get UI element Button_Cancel");

    if (connect_)
        SubscribeToEvent(connect_, E_RELEASED, URHO3D_HANDLER(ConnectPrompt, HandleButtonConnect));
    else
        URHO3D_LOGERROR("Failed to get UI element Button_Connect");

    LoadSettings();
}

// ----------------------------------------------------------------------------
void ConnectPrompt::SaveSettings()
{
    // Save the previously typed in values to XML so we don't have to type in
    // username/ip every time
    FileSystem* fs = GetSubsystem<FileSystem>();
    String url = fs->GetAppPreferencesDir("TheComet", "Asteroids");
    url.Append("ConnectPromptSettings.xml");

    XMLFile xml(context_);
    XMLElement settings = xml.CreateRoot("settings");
    settings.CreateChild("username").SetValue(username_->GetText());
    settings.CreateChild("address").SetValue(ipAddress_->GetText());
    settings.CreateChild("port").SetValue(port_->GetText());

    File f(context_);
    if (f.Open(url, FILE_WRITE) == false)
        return;
    URHO3D_LOGDEBUGF("Saving connect prompt settings to %s", url.CString());
    xml.Save(f);
}

// ----------------------------------------------------------------------------
void ConnectPrompt::LoadSettings()
{
    FileSystem* fs = GetSubsystem<FileSystem>();
    String url = fs->GetAppPreferencesDir("TheComet", "Asteroids");
    url.Append("ConnectPromptSettings.xml");

    File f(context_);
    if (f.Open(url, FILE_READ) == false)
        return;
    XMLFile xml(context_);
    xml.Load(f);

    XMLElement settings = xml.GetRoot();
    username_->SetText(settings.GetChild("username").GetValue());
    ipAddress_->SetText(settings.GetChild("address").GetValue());
    port_->SetText(settings.GetChild("port").GetValue());
}

// ----------------------------------------------------------------------------
void ConnectPrompt::InitiateConnectionProcess()
{
    // Urho3D doesn't like multiple pending Network::Connect() calls. Disable
    // the connect button to prevent this
    if (connect_->IsEnabled() == false)
        return;
    connect_->SetEnabled(false);

    if (port_->GetText().Length() == 0)
        port_->SetText(String(DEFAULT_PORT));

    SaveSettings();
    SubscribeToRegistryEvents();

    info_->SetText("Connecting to server...");
    info_->SetColor(Color::WHITE);

    // The Network::Connect() call needs to be given a pointer to the scene,
    // but we aren't responsible for managing the scene. Therefore, send an
    // event requesting to be connected so another system can do it for us.
    using namespace ConnectPromptRequestConnect;
    VariantMap& eventData = GetEventDataMap();
    eventData[P_USERNAME] = username_->GetText();
    eventData[P_ADDRESS] = ipAddress_->GetText();
    eventData[P_PORT] = ToUInt(port_->GetText());
    SendEvent(E_CONNECTPROMPTREQUESTCONNECT, eventData);
}

// ----------------------------------------------------------------------------
void ConnectPrompt::CancelConnectionProcess()
{
    if (connect_->IsEnabled())
        return;
    connect_->SetEnabled(true);

    UnsubscribeFromRegistryEvents();
    SendEvent(E_CONNECTPROMPTREQUESTCANCEL);
}

// ----------------------------------------------------------------------------
void ConnectPrompt::HandleButtonCancel(StringHash eventType, VariantMap& eventData)
{
    CancelConnectionProcess();
}

// ----------------------------------------------------------------------------
void ConnectPrompt::HandleButtonConnect(StringHash eventType, VariantMap& eventData)
{
    InitiateConnectionProcess();
}

// ----------------------------------------------------------------------------
void ConnectPrompt::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyDown;

    switch (eventData[P_KEY].GetInt())
    {
        case KEY_ESCAPE  : CancelConnectionProcess();
        case KEY_RETURN  :
        case KEY_RETURN2 : InitiateConnectionProcess();
    }
}

// ----------------------------------------------------------------------------
void ConnectPrompt::HandleTextChanged(StringHash eventType, VariantMap& eventData)
{
    // Only enable the connect button if the username and ip address text fields
    // contain text
    if (username_->GetText().Length() == 0 || ipAddress_->GetText().Length() == 0)
        connect_->SetEnabled(false);
    else
        connect_->SetEnabled(true);
}

// ----------------------------------------------------------------------------
void ConnectPrompt::SubscribeToRegistryEvents()
{
    SubscribeToEvent(E_REGISTERFAILED, URHO3D_HANDLER(ConnectPrompt, HandleRegisterFailed));
    SubscribeToEvent(E_REGISTERSUCCEEDED, URHO3D_HANDLER(ConnectPrompt, HandleRegisterSucceeded));
}

// ----------------------------------------------------------------------------
void ConnectPrompt::UnsubscribeFromRegistryEvents()
{
    UnsubscribeFromEvent(E_REGISTERFAILED);
    UnsubscribeFromEvent(E_REGISTERSUCCEEDED);
}

// ----------------------------------------------------------------------------
void ConnectPrompt::HandleRegisterFailed(StringHash eventType, VariantMap& eventData)
{
    info_->SetText("Error: " + eventData[RegisterFailed::P_REASON].GetString());
    info_->SetColor(Color::RED);
    UnsubscribeFromRegistryEvents();
    connect_->SetEnabled(true);
}

// ----------------------------------------------------------------------------
void ConnectPrompt::HandleRegisterSucceeded(StringHash eventType, VariantMap& eventData)
{
    connect_->SetEnabled(true);
    UnsubscribeFromRegistryEvents();
    SendEvent(E_CONNECTPROMPTSUCCESS);
}

}
