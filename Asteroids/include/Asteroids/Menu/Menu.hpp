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
    Menu(Urho3D::Context* context);

    void StartMainMenu();
    void StartPauseMenu();

private:
    void HandleMainMenuConnect(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleMainMenuQuit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleConnectPromptCancel(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleConnectPromptSuccess(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    MenuScreen* mainMenu_;
    MenuScreen* connectPrompt_;
};

}
