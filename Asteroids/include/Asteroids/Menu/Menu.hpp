#pragma once

#include "Asteroids/Config.hpp"
#include <Urho3D/Core/Object.h>

namespace Urho3D {
    class Scene;
}

namespace Asteroids {

class MenuScreen;

class ASTEROIDS_PUBLIC_API Menu : public Urho3D::Object
{
    URHO3D_OBJECT(Menu, Urho3D::Object)

public:
    enum Screen
    {
        MAIN_MENU,
        CONNECT_PROMPT1,  // Windows defines CONNECT_PROMPT somewhere so we can't use that
        HOST_SERVER_PROMPT,

        SCREEN_COUNT
    };

    Menu(Urho3D::Context* context);

    void StartMainMenu();
    void StartPauseMenu();
    void SwitchToScreen(Screen screen);
    void HideAllScreens();

private:
    void HandleMainMenuConnect(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleMainMenuHost(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleMainMenuQuit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleConnectPromptCancel(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleConnectPromptSuccess(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleHostServerPromptCancel(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleHostServerPromptSuccess(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    MenuScreen* screens_[SCREEN_COUNT];
};

}
