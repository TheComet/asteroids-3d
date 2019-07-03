#pragma once

#include <Urho3D/UI/UIElement.h>

namespace Asteroids {

class MenuScreen : public Urho3D::UIElement
{
    URHO3D_OBJECT(MenuScreen, Urho3D::UIElement)

public:
    MenuScreen(Urho3D::Context* context);

    virtual void Show() = 0;
    virtual void Hide() = 0;
};

}
