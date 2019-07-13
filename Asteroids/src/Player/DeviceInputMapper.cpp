#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Player/ActionState.hpp"
#include "Asteroids/Player/DeviceInputMapper.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/ResourceEvents.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
DeviceInputMapper::DeviceInputMapper (Context* context) :
    Component(context)
{
    SubscribeToEvent(E_JOYSTICKCONNECTED, URHO3D_HANDLER( DeviceInputMapper, HandleJoystickConnected));
    SubscribeToEvent(E_JOYSTICKDISCONNECTED, URHO3D_HANDLER( DeviceInputMapper, HandleJoystickDisconnected));
    SubscribeToEvent(E_JOYSTICKBUTTONDOWN, URHO3D_HANDLER( DeviceInputMapper, HandleJoystickButtonDown));
    SubscribeToEvent(E_JOYSTICKBUTTONUP, URHO3D_HANDLER( DeviceInputMapper, HandleJoystickButtonUp));
    SubscribeToEvent(E_JOYSTICKAXISMOVE, URHO3D_HANDLER( DeviceInputMapper, HandleJoystickAxisMove));
    SubscribeToEvent(E_JOYSTICKHATMOVE, URHO3D_HANDLER( DeviceInputMapper, HandleJoystickHatMove));
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER( DeviceInputMapper, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, URHO3D_HANDLER( DeviceInputMapper, HandleKeyUp));
}

// ----------------------------------------------------------------------------
void DeviceInputMapper::RegisterObject(Context* context)
{
    context->RegisterFactory<DeviceInputMapper>(ASTEROIDS_CATEGORY);

    URHO3D_ACCESSOR_ATTRIBUTE("Input Map Config", GetConfigAttr, SetConfigAttr, ResourceRef, ResourceRef(XMLFile::GetTypeStatic()), AM_DEFAULT);
}

// ----------------------------------------------------------------------------
ResourceRef DeviceInputMapper::GetConfigAttr() const
{
    return GetResourceRef(configFile_, XMLFile::GetTypeStatic());
}

// ----------------------------------------------------------------------------
void DeviceInputMapper::SetConfigAttr(const ResourceRef& value)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    SetConfig(cache->GetResource<XMLFile>(value.name_));
}

// ----------------------------------------------------------------------------
void DeviceInputMapper::SetConfig(Urho3D::XMLFile* mappingConfig)
{
    if (configFile_)
    {
        UnsubscribeFromEvent(E_FILECHANGED);
    }

    configFile_ = mappingConfig;

    if (configFile_)
    {
        SubscribeToEvent(E_FILECHANGED, URHO3D_HANDLER( DeviceInputMapper, HandleFileChanged));
        UpdateMappingFromConfig();
    }
}

// ----------------------------------------------------------------------------
void DeviceInputMapper::UpdateMappingFromConfig()
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
void DeviceInputMapper::HandleJoystickConnected(StringHash eventType, VariantMap& eventData)
{
    using namespace JoystickConnected;
    int id = eventData[P_JOYSTICKID].GetInt();
    JoystickState* js = GetSubsystem<Input>()->GetJoystick(id);
    URHO3D_LOGINFOF("Connected joystick \"%s\", id: %d", js->name_.CString(), id);

    if (configFile_)
        UpdateMappingFromConfig();
}

// ----------------------------------------------------------------------------
void DeviceInputMapper::HandleJoystickDisconnected(StringHash eventType, VariantMap& eventData)
{
    using namespace JoystickDisconnected;
    int id = eventData[P_JOYSTICKID].GetInt();
    URHO3D_LOGINFOF("Disconnected joystick id: %d", id);

    if (configFile_)
        UpdateMappingFromConfig();
}

// ----------------------------------------------------------------------------
void DeviceInputMapper::HandleJoystickButtonDown(StringHash eventType, VariantMap& eventData)
{
    using namespace JoystickButtonDown;

    int joyID = eventData[P_JOYSTICKID].GetInt();
    int buttonID = eventData[P_BUTTON].GetInt();
    ActionState* state = GetComponent<ActionState>();
    if (state == nullptr)
        return;

    for (const Mapping& mapping : mappings_)
        if (mapping.deviceID == joyID && mapping.type == T_BUTTON && mapping.buttonID == buttonID)
        {
            switch(mapping.actionID)
            {
                case A_LEFT    : state->SetLeft(1.0);       break;
                case A_RIGHT   : state->SetRight(1.0);      break;
                case A_THRUST  : state->SetThrusting(true); break;
                case A_FIRE    : state->SetFiring(true);    break;
                case A_WARP    : state->SetWarp(true);      break;
                case A_USEITEM : state->SetUseItem(true);   break;
            }
            break;
        }
}

// ----------------------------------------------------------------------------
void DeviceInputMapper::HandleJoystickButtonUp(StringHash eventType, VariantMap& eventData)
{
    using namespace JoystickButtonUp;

    int joyID = eventData[P_JOYSTICKID].GetInt();
    int buttonID = eventData[P_BUTTON].GetInt();
    ActionState* state = GetComponent<ActionState>();
    if (state == nullptr)
        return;

    for (const Mapping& mapping : mappings_)
        if (mapping.deviceID == joyID && mapping.type == T_BUTTON && mapping.buttonID == buttonID)
        {
            switch(mapping.actionID)
            {
                case A_LEFT    : state->SetLeft(0.0);        break;
                case A_RIGHT   : state->SetRight(0.0);       break;
                case A_THRUST  : state->SetThrusting(false); break;
                case A_FIRE    : state->SetFiring(false);    break;
                case A_WARP    : state->SetWarp(false);      break;
                case A_USEITEM : state->SetUseItem(false);   break;
            }
            break;
        }
}

// ----------------------------------------------------------------------------
void DeviceInputMapper::HandleJoystickAxisMove(StringHash eventType, VariantMap& eventData)
{
    using namespace JoystickAxisMove;

    int joyID = eventData[P_JOYSTICKID].GetInt();
    int axisID = eventData[P_AXIS].GetInt();
    float position = eventData[P_POSITION].GetFloat();
    const float threshold = 0.4;
    ActionState* state = GetComponent<ActionState>();
    if (state == nullptr)
        return;

    for (const Mapping& mapping : mappings_)
        if (mapping.deviceID == joyID && mapping.type == T_AXIS && mapping.buttonID == axisID)
        {
            switch(mapping.actionID)
            {
                case A_LEFT    : state->SetLeft(Max(position * mapping.position, 0)); break;
                case A_RIGHT   : state->SetRight(Max(position * mapping.position, 0)); break;
                case A_FIRE    : state->SetFiring(Max(position * mapping.position, 0) > threshold); break;
                case A_THRUST  : state->SetThrusting(Max(position * mapping.position, 0) > threshold); break;
                case A_WARP    : state->SetWarp(Max(position * mapping.position, 0) > threshold); break;
                case A_USEITEM : state->SetUseItem(Max(position * mapping.position, 0) > threshold); break;
            }
        }
}

// ----------------------------------------------------------------------------
void DeviceInputMapper::HandleJoystickHatMove(StringHash eventType, VariantMap& eventData)
{
    using namespace JoystickHatMove;

    int joyID = eventData[P_JOYSTICKID].GetInt();
    int hatID = eventData[P_HAT].GetInt();
    int position = eventData[P_POSITION].GetInt();
    ActionState* state = GetComponent<ActionState>();
    if (state == nullptr)
        return;

    for (const Mapping& mapping : mappings_)
        if (mapping.deviceID == joyID && mapping.type == T_HAT && mapping.buttonID == hatID)
        {
            switch(mapping.actionID)
            {
                case A_LEFT    : state->SetLeft(position & mapping.position ? 1.0 : 0.0);  break;
                case A_RIGHT   : state->SetRight(position & mapping.position ? 1.0 : 0.0); break;
                case A_FIRE    : state->SetFiring(position & mapping.position ? true : false); break;
                case A_THRUST  : state->SetThrusting(position & mapping.position ? true : false); break;
                case A_WARP    : state->SetWarp(position & mapping.position ? true : false); break;
                case A_USEITEM : state->SetUseItem(position & mapping.position ? true : false); break;
            }
        }
}

// ----------------------------------------------------------------------------
void DeviceInputMapper::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyDown;

    int keyID = eventData[P_KEY].GetInt();
    ActionState* state = GetComponent<ActionState>();
    if (state == nullptr)
        return;

    for (const Mapping& mapping : mappings_)
        if (mapping.deviceID == -1 && mapping.type == T_KEY && mapping.buttonID == keyID)
        {
            switch(mapping.actionID)
            {
                case A_LEFT    : state->SetLeft(1.0);       break;
                case A_RIGHT   : state->SetRight(1.0);      break;
                case A_THRUST  : state->SetThrusting(true); break;
                case A_FIRE    : state->SetFiring(true);    break;
                case A_WARP    : state->SetWarp(true);      break;
                case A_USEITEM : state->SetUseItem(true);   break;
            }
            break;
        }
}

// ----------------------------------------------------------------------------
void DeviceInputMapper::HandleKeyUp(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyUp;

    int keyID = eventData[P_KEY].GetInt();
    ActionState* state = GetComponent<ActionState>();
    if (state == nullptr)
        return;

    for (const Mapping& mapping : mappings_)
        if (mapping.deviceID == -1 && mapping.type == T_KEY && mapping.buttonID == keyID)
        {
            switch(mapping.actionID)
            {
                case A_LEFT    : state->SetLeft(0.0);        break;
                case A_RIGHT   : state->SetRight(0.0);       break;
                case A_THRUST  : state->SetThrusting(false); break;
                case A_FIRE    : state->SetFiring(false);    break;
                case A_WARP    : state->SetWarp(false);      break;
                case A_USEITEM : state->SetUseItem(false);   break;
            }
            break;
        }
}

// ----------------------------------------------------------------------------
void DeviceInputMapper::HandleFileChanged(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    using namespace FileChanged;

    if (configFile_->GetName() == eventData[P_RESOURCENAME].GetString())
    {
        UpdateMappingFromConfig();
    }
}

}
