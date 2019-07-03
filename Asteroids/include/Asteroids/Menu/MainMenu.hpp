#pragma once

#include "Asteroids/Config.hpp"
#include "Asteroids/Menu/MenuScreen.hpp"

namespace Asteroids {

class MainMenu : public MenuScreen
{
    URHO3D_OBJECT(MainMenu, MenuScreen)

public:
    MainMenu(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

    void Show() override;
    void Hide() override;

private:
    void LoadXMLAndInit();
    void HandleConnect(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleOptions(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleQuit(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    Urho3D::SharedPtr<Urho3D::XMLFile> xmlFile_;
};

}
