#pragma once

#include "Asteroids/Config.hpp"
#include <Urho3D/Scene/Component.h>

namespace Asteroids {

class ActionState;

class WeaponSpawner : public Urho3D::Component
{
    URHO3D_OBJECT(WeaponSpawner, Urho3D::Component)

public:
    WeaponSpawner(Urho3D::Context* context);
    static void RegisterObject(Urho3D::Context* context);

    void CreateBullet();

private:
    void ParseConfig();
    bool TryGetActionState();
    void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleActionWarp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleActionUseItem(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    struct
    {
        struct
        {
            float speed = 0;
            float life = 0;
            float cooldown = 0;
            float initialOffset = 0;
        } bullet;
    } config_;

    Urho3D::WeakPtr<ActionState> state_;
    Urho3D::SharedPtr<Urho3D::XMLFile> configXML_;
    float fireActionCooldown_;
};

}
