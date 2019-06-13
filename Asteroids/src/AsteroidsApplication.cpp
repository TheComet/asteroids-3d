#include "Asteroids/AsteroidsApplication.hpp"
#include "Asteroids/DebugTextScroll.hpp"

#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/UI/UI.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
AsteroidsApplication::AsteroidsApplication(Context* context) :
    Application(context)
{
}

// ----------------------------------------------------------------------------
void AsteroidsApplication::Setup()
{
    engineParameters_["FullScreen"]      = false;
    engineParameters_["WindowResizable"] = true;
    engineParameters_["VSync"]           = true;
    engineParameters_["Multisample"]     = 2;
}

// ----------------------------------------------------------------------------
void AsteroidsApplication::Start()
{
    RegisterStuff();
#if defined(DEBUG)
    CreateDebugHud();
#endif

    // Configure resource cache to auto-reload resources when they change on
    // the filesystem
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    cache->SetAutoReloadResources(true);

    // UI style
    XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
    UI* ui = GetSubsystem<UI>();
    if (style && ui)
        ui->GetRoot()->SetDefaultStyle(style);

    // Mouse should be visible by default
    GetSubsystem<Input>()->SetMouseVisible(true);

#if defined(DEBUG)
    GetSubsystem<Log>()->SetLevel(LOG_DEBUG);
#endif

    SubscribeToEvents();
}

// ----------------------------------------------------------------------------
void AsteroidsApplication::Stop()
{

}

// ----------------------------------------------------------------------------
void AsteroidsApplication::RegisterStuff()
{
#if defined(DEBUG)
    context_->RegisterSubsystem(new DebugTextScroll(context_));
    GetSubsystem<DebugTextScroll>()->SetTextCount(40);
#endif
}

// ----------------------------------------------------------------------------
void AsteroidsApplication::CreateDebugHud()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
    debugHud_ = engine_->CreateDebugHud();
    if (debugHud_)
        debugHud_->SetDefaultStyle(style);
}

// ----------------------------------------------------------------------------
void AsteroidsApplication::SubscribeToEvents()
{
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(AsteroidsApplication, HandleKeyDown));
}

// ----------------------------------------------------------------------------
void AsteroidsApplication::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    using namespace KeyDown;

    int key = eventData[P_KEY].GetInt();

    // Exit game
    if (key == KEY_ESCAPE)
        engine_->Exit();

    // Toggle debug HUD
    if (key == KEY_F2)
    {
        if (debugHud_->GetMode() == DEBUGHUD_SHOW_NONE)
            debugHud_->SetMode(DEBUGHUD_SHOW_ALL);
        else if (debugHud_->GetMode() == DEBUGHUD_SHOW_ALL)
            debugHud_->SetMode(DEBUGHUD_SHOW_MEMORY);
        else
            debugHud_->SetMode(DEBUGHUD_SHOW_NONE);
    }
}

}
