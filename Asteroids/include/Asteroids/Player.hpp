#pragma once

#include <Urho3D/Scene/Component.h>

namespace Asteroids {

class InputActionMapper;

class Player : public Urho3D::Component
{
    URHO3D_OBJECT(Player, Urho3D::Component)

public:
    Player(Urho3D::Context* context);

    static void RegisterObject(Urho3D::Context* context);

    void SetInputActionMapper(InputActionMapper* mapper);

private:
    void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleActionWarp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleActionUseItem(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    Urho3D::SharedPtr<InputActionMapper> mapper_;
};

}
