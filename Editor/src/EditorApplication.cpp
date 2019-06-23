#include "Editor/EditorApplication.hpp"
#include "Asteroids/AsteroidsLib.hpp"

#include <Urho3D/AngelScript/Script.h>
#include <Urho3D/AngelScript/ScriptFile.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Resource/ResourceCache.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
EditorApplication::EditorApplication(Context* context) :
    Application(context)
{
}

// ----------------------------------------------------------------------------
void EditorApplication::Setup()
{
    engineParameters_[EP_LOG_NAME]         = "asteroids-editor.log";
    engineParameters_[EP_FULL_SCREEN]      = false;
    engineParameters_[EP_WINDOW_RESIZABLE] = true;
    engineParameters_[EP_VSYNC]            = true;
    engineParameters_[EP_MULTI_SAMPLE]     = 2;
    engineParameters_[EP_RESOURCE_PATHS]   = "CoreData;EditorData";
}

// ----------------------------------------------------------------------------
void EditorApplication::Start()
{
    RegisterObjectFactories(context_);
    context_->RegisterSubsystem<Script>();

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    script_ = cache->GetResource<ScriptFile>("Scripts/Editor.as");
    if (!script_ || !script_->Execute("void Start()"))
        ErrorExit();
}

// ----------------------------------------------------------------------------
void EditorApplication::Stop()
{
    if (script_ && script_->GetFunction("void Stop()"))
        script_->Execute("void Stop()");
}

}
