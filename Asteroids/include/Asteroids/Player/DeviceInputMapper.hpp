#pragma once

#include "Asteroids/Config.hpp"
#include <Urho3D/Scene/Component.h>

namespace Urho3D {
    class XMLFile;
}

namespace Asteroids {

class ASTEROIDS_PUBLIC_API DeviceInputMapper : public Urho3D::Component
{
    URHO3D_OBJECT( DeviceInputMapper, Urho3D::Component)

public:
    DeviceInputMapper (Urho3D::Context* context);

    static void RegisterObject(Urho3D::Context* context);

    /*!
     * @brief Sets the config file to use for all of the button mappings.
     * The file is auto-reloaded if changes are made.
     */
    void SetConfig(Urho3D::XMLFile* mappingConfig);

    Urho3D::ResourceRef GetConfigAttr() const;
    void SetConfigAttr(const Urho3D::ResourceRef& value);

private:
    void HandleJoystickConnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleJoystickDisconnected(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleJoystickButtonDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleJoystickButtonUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleJoystickAxisMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleJoystickHatMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    void HandleKeyDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void HandleKeyUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);

    void HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData);
    void UpdateMappingFromConfig();

    enum ActionID
    {
        A_LEFT,
        A_RIGHT,
        A_FIRE,
        A_THRUST,
        A_WARP,
        A_USEITEM
    };

    enum InputType
    {
        T_KEY,
        T_BUTTON,
        T_AXIS,
        T_HAT
    };

    struct Mapping
    {
        int deviceID;
        int buttonID;
        int position;  // needed for joystick hat and axis direction
        InputType type;
        ActionID actionID;
    };

    Urho3D::SharedPtr<Urho3D::XMLFile> configFile_;
    Urho3D::PODVector<Mapping> mappings_;
};

}
