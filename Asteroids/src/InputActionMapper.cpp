#include "Asteroids/InputActionMapper.hpp"
#include "Asteroids/InputActionMapperEvents.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceEvents.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
InputActionMapper::InputActionMapper(Context* context) :
    Component(context),
    state_({})
{
    SubscribeToEvent(E_JOYSTICKCONNECTED, URHO3D_HANDLER(InputActionMapper, HandleJoystickConnected));
    SubscribeToEvent(E_JOYSTICKDISCONNECTED, URHO3D_HANDLER(InputActionMapper, HandleJoystickDisconnected));
    SubscribeToEvent(E_JOYSTICKBUTTONDOWN, URHO3D_HANDLER(InputActionMapper, HandleJoystickButtonDown));
    SubscribeToEvent(E_JOYSTICKBUTTONUP, URHO3D_HANDLER(InputActionMapper, HandleJoystickButtonUp));
    SubscribeToEvent(E_JOYSTICKAXISMOVE, URHO3D_HANDLER(InputActionMapper, HandleJoystickAxisMove));
    SubscribeToEvent(E_JOYSTICKHATMOVE, URHO3D_HANDLER(InputActionMapper, HandleJoystickHatMove));
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(InputActionMapper, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(InputActionMapper, HandleKeyUp));
}

// ----------------------------------------------------------------------------
void InputActionMapper::RegisterObject(Context* context)
{
    context->RegisterFactory<InputActionMapper>();
}

// ----------------------------------------------------------------------------
void InputActionMapper::SetConfig(Urho3D::XMLFile* mappingConfig)
{
    if (configFile_)
    {
        UnsubscribeFromEvent(E_FILECHANGED);
    }

    configFile_ = mappingConfig;

    if (configFile_)
    {
        SubscribeToEvent(E_FILECHANGED, URHO3D_HANDLER(InputActionMapper, HandleFileChanged));
        UpdateMappingFromConfig();
    }
}

// ----------------------------------------------------------------------------
void InputActionMapper::UpdateMappingFromConfig()
{
    mappings_.Clear();

    XMLElement devices = configFile_->GetRoot();
    for (XMLElement device = devices.GetChild("device"); device; device = device.GetNext("device"))
    {
        Mapping mapping = {0};

        // Determine the device ID by scanning all connected devices. Kind of
        // hackish: The keyboard is considered id=-1 because joystick IDs start
        // at ID 0.
        mapping.deviceID = -1;
        String deviceName = device.GetAttribute("name");
        Input* input = GetSubsystem<Input>();
        for (int i = 0; i != input->GetNumJoysticks(); ++i)
        {
            JoystickState* js = input->GetJoystickByIndex(i);
            if (deviceName == js->name_)
            {
                mapping.deviceID = js->joystickID_;
                break;
            }
        }
        if (mapping.deviceID == -1 && deviceName != "keyboard")
        {
            URHO3D_LOGDEBUGF("Device \"%s\" is not connected, skip reading mappings", deviceName.CString());
            continue;
        }

        // Map all actions to buttons
        for (XMLElement action = device.GetChild("action"); action; action = action.GetNext("action"))
        {
            // Determine the action being mapped
            String actionName = action.GetAttribute("name");
            if (actionName == "Left")         mapping.actionID = A_LEFT;
            else if (actionName == "Right")   mapping.actionID = A_RIGHT;
            else if (actionName == "Fire")    mapping.actionID = A_FIRE;
            else if (actionName == "Thrust")  mapping.actionID = A_THRUST;
            else if (actionName == "Warp")    mapping.actionID = A_WARP;
            else if (actionName == "UseItem") mapping.actionID = A_USEITEM;
            else
            {
                URHO3D_LOGERRORF("Unknown action \"%s\" while reading config file \"%s\"", actionName.CString(), configFile_->GetName().CString());
                continue;
            }

            // Determine the type of input (keyboard, joy button, joy axis, or joy hat)
            if (action.HasAttribute("key"))         { mapping.type = T_KEY;    mapping.buttonID = action.GetInt("key"); }
            else if (action.HasAttribute("button")) { mapping.type = T_BUTTON; mapping.buttonID = action.GetInt("button"); }
            else if (action.HasAttribute("axis") && action.HasAttribute("sign"))    { mapping.type = T_AXIS; mapping.buttonID = action.GetInt("axis"); mapping.position = (action.GetInt("sign") > 0)*2-1; }
            else if (action.HasAttribute("hat") && action.HasAttribute("position")) { mapping.type = T_HAT;  mapping.buttonID = action.GetInt("hat"); mapping.position = action.GetInt("position"); }
            else
            {
                URHO3D_LOGERRORF("Unknown input type, or no input was specified, for action \"%s\" device \"%s\" while reading config file \"%s\"", actionName.CString(), deviceName.CString(), configFile_->GetName().CString());
                continue;
            }

            mappings_.Push(mapping);
        }
    }
}

// ----------------------------------------------------------------------------
uint16_t InputActionMapper::GetState() const
{
    return state_.bits;
}

// ----------------------------------------------------------------------------
void InputActionMapper::SetState(uint16_t newState)
{
    VariantMap& eventData = GetEventDataMap();
    uint16_t posEdge = newState & ~state_.bits;

    if (posEdge & 0x4000) SendEvent(E_ACTIONWARP, eventData);
    if (posEdge & 0x8000) SendEvent(E_ACTIONUSEITEM, eventData);

    state_.bits = newState;
}

// ----------------------------------------------------------------------------
float InputActionMapper::GetLeft() const
{
    return float(state_.field.left) / 0x3F;
}

// ----------------------------------------------------------------------------
float InputActionMapper::GetRight() const
{
    return float(state_.field.right) / 0x3F;
}

// ----------------------------------------------------------------------------
bool InputActionMapper::IsFiring() const
{
    return (state_.field.fire == 1);
}

// ----------------------------------------------------------------------------
bool InputActionMapper::IsThrusting() const
{
    return (state_.field.thrust == 1);
}

// ----------------------------------------------------------------------------
void InputActionMapper::HandleJoystickConnected(StringHash eventType, VariantMap& eventData)
{
    using namespace JoystickConnected;
    int id = eventData[P_JOYSTICKID].GetInt();
    JoystickState* js = GetSubsystem<Input>()->GetJoystick(id);
    URHO3D_LOGINFOF("Connected joystick \"%s\", id: %d", js->name_.CString(), id);

    if (configFile_)
        UpdateMappingFromConfig();
}

// ----------------------------------------------------------------------------
void InputActionMapper::HandleJoystickDisconnected(StringHash eventType, VariantMap& eventData)
{
    using namespace JoystickDisconnected;
    int id = eventData[P_JOYSTICKID].GetInt();
    URHO3D_LOGINFOF("Disconnected joystick id: %d", id);

    if (configFile_)
        UpdateMappingFromConfig();
}

// ----------------------------------------------------------------------------
void InputActionMapper::HandleJoystickButtonDown(StringHash eventType, VariantMap& eventData)
{
    using namespace JoystickButtonDown;

    int joyID = eventData[P_JOYSTICKID].GetInt();
    int buttonID = eventData[P_BUTTON].GetInt();

    State newState = state_;
    for (const Mapping& mapping : mappings_)
        if (mapping.deviceID == joyID && mapping.type == T_BUTTON && mapping.buttonID == buttonID)
        {
            switch(mapping.actionID)
            {
                case A_LEFT    : newState.field.left = 0x3F;  break; // full throttle (6 bits)
                case A_RIGHT   : newState.field.right = 0x3F; break; // full throttle (6 bits)
                case A_THRUST  : newState.field.thrust = 1;   break;
                case A_FIRE    : newState.field.fire = 1; break;
                case A_WARP    : newState.field.warp = 1; break;
                case A_USEITEM : newState.field.useItem = 1; break;
            }
            SetState(newState.bits);
            break;
        }
}

// ----------------------------------------------------------------------------
void InputActionMapper::HandleJoystickButtonUp(StringHash eventType, VariantMap& eventData)
{
    using namespace JoystickButtonUp;

    int joyID = eventData[P_JOYSTICKID].GetInt();
    int buttonID = eventData[P_BUTTON].GetInt();

    State newState = state_;
    for (const Mapping& mapping : mappings_)
        if (mapping.deviceID == joyID && mapping.type == T_BUTTON && mapping.buttonID == buttonID)
        {
            switch(mapping.actionID)
            {
                case A_LEFT    : newState.field.left = 0;     break;
                case A_RIGHT   : newState.field.right = 0;    break;
                case A_FIRE    : newState.field.fire = 0;     break;
                case A_THRUST  : newState.field.thrust = 0;   break;
                case A_WARP    : newState.field.warp = 0;     break;
                case A_USEITEM : newState.field.useItem = 0;  break;
            }
            SetState(newState.bits);
            break;
        }
}

// ----------------------------------------------------------------------------
void InputActionMapper::HandleJoystickAxisMove(StringHash eventType, VariantMap& eventData)
{
    using namespace JoystickAxisMove;

    int joyID = eventData[P_JOYSTICKID].GetInt();
    int axisID = eventData[P_AXIS].GetInt();
    float position = eventData[P_POSITION].GetFloat();
    const float threshold = 0.4;

    State newState = state_;
    for (const Mapping& mapping : mappings_)
        if (mapping.deviceID == joyID && mapping.type == T_AXIS && mapping.buttonID == axisID)
        {
            switch(mapping.actionID)
            {
                case A_LEFT    : newState.field.left = unsigned(Max(position * mapping.position, 0) * 0x3F);  break;
                case A_RIGHT   : newState.field.right = unsigned(Max(position * mapping.position, 0) * 0x3F); break;
                case A_FIRE    : newState.field.fire = (Max(position * mapping.position, 0) > threshold);     break;
                case A_THRUST  : newState.field.thrust = (Max(position * mapping.position, 0) > threshold);   break;
                case A_WARP    : newState.field.warp = (Max(position * mapping.position, 0) > threshold);     break;
                case A_USEITEM : newState.field.useItem = (Max(position * mapping.position, 0) > threshold);  break;
            }
            SetState(newState.bits);
        }
}

// ----------------------------------------------------------------------------
void InputActionMapper::HandleJoystickHatMove(StringHash eventType, VariantMap& eventData)
{
    using namespace JoystickHatMove;

    int joyID = eventData[P_JOYSTICKID].GetInt();
    int hatID = eventData[P_HAT].GetInt();
    int position = eventData[P_POSITION].GetInt();

    State newState = state_;
    for (const Mapping& mapping : mappings_)
        if (mapping.deviceID == joyID && mapping.type == T_HAT && mapping.buttonID == hatID)
        {
            switch(mapping.actionID)
            {
                case A_LEFT    : newState.field.left = (position & mapping.position) * 0x3F;  break; // full throttle (6 bits)
                case A_RIGHT   : newState.field.right = (position & mapping.position) * 0x3F; break; // full throttle (6 bits)
                case A_FIRE    : newState.field.fire = (position & mapping.position);     break;
                case A_THRUST  : newState.field.thrust = (position & mapping.position);   break;
                case A_WARP    : newState.field.warp = (position & mapping.position);     break;
                case A_USEITEM : newState.field.useItem = (position & mapping.position);  break;
            }
            SetState(newState.bits);
        }
}

// ----------------------------------------------------------------------------
void InputActionMapper::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyDown;

    int keyID = eventData[P_KEY].GetInt();

    State newState = state_;
    for (const Mapping& mapping : mappings_)
        if (mapping.deviceID == -1 && mapping.type == T_KEY && mapping.buttonID == keyID)
        {
            switch(mapping.actionID)
            {
                case A_LEFT    : newState.field.left = 0x3F;  break; // full throttle (6 bits)
                case A_RIGHT   : newState.field.right = 0x3F; break; // full throttle (6 bits)
                case A_FIRE    : newState.field.fire = 1;     break;
                case A_THRUST  : newState.field.thrust = 1;   break;
                case A_WARP    : newState.field.warp = 1;     break;
                case A_USEITEM : newState.field.useItem = 1;  break;
            }
            SetState(newState.bits);
            break;
        }
}

// ----------------------------------------------------------------------------
void InputActionMapper::HandleKeyUp(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyUp;

    int keyID = eventData[P_KEY].GetInt();

    State newState = state_;
    for (const Mapping& mapping : mappings_)
        if (mapping.deviceID == -1 && mapping.type == T_KEY && mapping.buttonID == keyID)
        {
            switch(mapping.actionID)
            {
                case A_LEFT    : newState.field.left = 0;     break;
                case A_RIGHT   : newState.field.right = 0;    break;
                case A_FIRE    : newState.field.fire = 0;     break;
                case A_THRUST  : newState.field.thrust = 0;   break;
                case A_WARP    : newState.field.warp = 0;     break;
                case A_USEITEM : newState.field.useItem = 0;  break;
            }
            SetState(newState.bits);
            break;
        }
}

// ----------------------------------------------------------------------------
void InputActionMapper::HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    using namespace FileChanged;

    if (configFile_->GetName() == eventData[P_RESOURCENAME].GetString())
    {
        UpdateMappingFromConfig();
    }
}

}
