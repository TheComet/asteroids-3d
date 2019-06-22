#pragma once

#include "Asteroids/Config.hpp"
#include "Asteroids/SurfaceObject.hpp"

namespace Urho3D
{
    class XMLFile;
}

namespace Asteroids {

class InputActionMapper;

class ASTEROIDS_PUBLIC_API Player : public SurfaceObject
{
    URHO3D_OBJECT(Player, SurfaceObject)

public:
    Player(Urho3D::Context* context);

    static Player* Create(Urho3D::Scene* scene);
    static void Destroy(Player* player);

    void SetConfig(Urho3D::XMLFile* config);

    void ListenToMapper(InputActionMapper* mapper);

private:
    void SubscribeToEvents();
    void ReadShipConfig();
    void HandleUpdate(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleActionWarp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleActionUseItem(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

private:
    struct ShipConfig
    {
        float rotationSpeed_;
        float acceleration_;
        float maxVelocity_;
        float velocityDecay_;
    } shipConfig_;

    Urho3D::WeakPtr<InputActionMapper> mapper_;
    Urho3D::SharedPtr<Urho3D::XMLFile> configFile_;
    Urho3D::Vector2 velocity_;
    float angle_;
    float shootCooldown_;
};

}
