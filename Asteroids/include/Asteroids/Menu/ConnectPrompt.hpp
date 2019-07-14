#pragma once

#include "Asteroids/Config.hpp"
#include "Asteroids/Menu/MenuScreen.hpp"

namespace Urho3D {
    class Button;
    class LineEdit;
    class Text;
}

namespace Asteroids {

class ConnectPrompt : public MenuScreen
{
    URHO3D_OBJECT(ConnectPrompt, MenuScreen)

public:
    ConnectPrompt(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

    void Show() override;
    void Hide() override;

private:
    void LoadXMLAndInit();
    void SaveSettings();
    void LoadSettings();
    void InitiateConnectionProcess();
    void CancelConnectionProcess();
    void LockInput();
    void UnlockInput();
    bool IsLocked();

    // UI element events
    void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleButtonCancel(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleButtonConnect(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleTextChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    // UserRegistry and network events when attempting to connect
    void SubscribeToRegistryEvents();
    void UnsubscribeFromRegistryEvents();
    void HandleRegisterFailed(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleRegisterSucceeded(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::SharedPtr<Urho3D::XMLFile> xmlFile_;

    Urho3D::Text* info_;
    Urho3D::LineEdit* username_;
    Urho3D::LineEdit* ipAddress_;
    Urho3D::LineEdit* port_;
    Urho3D::Button* cancel_;
    Urho3D::Button* connect_;
};

}
