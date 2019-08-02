#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Objects/PhaserController.hpp"
#include "Asteroids/Objects/MineController.hpp"
#include "Asteroids/Player/ActionState.hpp"
#include "Asteroids/Player/ActionStateEvents.hpp"
#include "Asteroids/Player/ShipController.hpp"
#include "Asteroids/Player/WeaponSpawner.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/ResourceEvents.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Resource/XMLElement.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
WeaponSpawner::WeaponSpawner(Context* context) :
    Component(context),
    fireActionCooldown_(0)
{
    configXML_ = GetSubsystem<ResourceCache>()->GetResource<XMLFile>("Config/WeaponSpawner.xml");
    ParseConfig();

    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(WeaponSpawner, HandleUpdate));
    SubscribeToEvent(E_FILECHANGED, URHO3D_HANDLER(WeaponSpawner, HandleFileChanged));
}

// ----------------------------------------------------------------------------
void WeaponSpawner::RegisterObject(Context* context)
{
    context->RegisterFactory<WeaponSpawner>(ASTEROIDS_CATEGORY);
}

// ----------------------------------------------------------------------------
void WeaponSpawner::CreatePhaser(float angleOffset)
{
    // Load bullet prefab
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Node* bullet = GetScene()->CreateChild();
    XMLFile* bulletXML = cache->GetResource<XMLFile>("Prefabs/Phaser.xml");
    bullet->LoadXML(bulletXML->GetRoot());

    // Calculate the effective bullet direction, which is a combination of the
    // player's angle and player's speed
    ShipController* shipController = GetComponent<ShipController>();
    Vector2 bulletStandingVelocity(Sin(shipController->GetAngle() + angleOffset) * config_.phaser.speed, Cos(shipController->GetAngle() + angleOffset) * config_.phaser.speed);
    Vector2 bulletVelocity = bulletStandingVelocity + shipController->GetVelocity();

    // Set up bullet controller with the correct speed/life parameters.
    // Ship controller should always exist if weapon spawner exists.
    PhaserController* phaserController = bullet->GetChild("Phaser")->GetComponent<PhaserController>();
    phaserController->SetLife(config_.phaser.life);
    phaserController->SetVelocity(bulletVelocity);

    // Set initial bullet location to the tip of the player's ship
    // Note: Have to update planet height before moving the bullet, as
    // UpdatePosition takes into account the planet's radius
    bullet->SetRotation(node_->GetParent()->GetRotation());
    phaserController->UpdatePlanetHeight();
    phaserController->UpdatePosition(phaserController->GetVelocity().Normalized(), config_.phaser.initialOffset);
}

// ----------------------------------------------------------------------------
void WeaponSpawner::CreateSpread()
{
    float angle = -config_.spread.spread / 2;
    float incr = config_.spread.spread / (config_.spread.count - 1);
    for (int i = 0; i != config_.spread.count; ++i, angle += incr)
        CreatePhaser(angle);
}

// ----------------------------------------------------------------------------
void WeaponSpawner::CreateMine()
{
    // Load mine prefab
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Node* mine = GetScene()->CreateChild();
    XMLFile* mineXML = cache->GetResource<XMLFile>("Prefabs/Mine.xml");
    mine->LoadXML(mineXML->GetRoot());

    // Calculate the effective mine direction, which is a combination of the
    // player's angle and player's speed
    ShipController* shipController = GetComponent<ShipController>();
    Vector2 mineStandingVelocity(-Sin(shipController->GetAngle()) * config_.mine.ejectSpeed, -Cos(shipController->GetAngle()) * config_.mine.ejectSpeed);
    Vector2 mineVelocity = mineStandingVelocity + shipController->GetVelocity();

    // Set up mine controller with the correct speed/life parameters.
    // Ship controller should always exist if weapon spawner exists.
    MineController* mineController = mine->GetChild("Mine")->GetComponent<MineController>();
    mineController->SetLife(config_.mine.life);
    mineController->SetVelocity(mineVelocity);
    mineController->SetDeceleration(config_.mine.deceleration);

    // Set initial mine location to the back of the player's ship
    // Note: Have to update planet height before moving the mine, as
    // UpdatePosition takes into account the planet's radius
    mine->SetRotation(node_->GetParent()->GetRotation());
    mineController->UpdatePlanetHeight();
    mineController->UpdatePosition(mineController->GetVelocity().Normalized(), config_.mine.initialOffset);
}

// ----------------------------------------------------------------------------
void WeaponSpawner::ParseConfig()
{
    if (configXML_ == nullptr)
        return;

    XMLElement root = configXML_->GetRoot();
    XMLElement phaser = root.GetChild("phaser");
    for (XMLElement param = phaser.GetChild("param"); param; param = param.GetNext("param"))
    {
        String name = param.GetAttribute("name");
        if      (name == "speed")         config_.phaser.speed = param.GetFloat("value");
        else if (name == "life")          config_.phaser.life = param.GetFloat("value");
        else if (name == "cooldown")      config_.phaser.cooldown = param.GetFloat("value");
        else if (name == "initialOffset") config_.phaser.initialOffset = param.GetFloat("value");
        else URHO3D_LOGERRORF("Unknown parameter bullet \"%s\" while reading config file \"%s\"", name.CString(), configXML_->GetName().CString());
    }

    XMLElement bulletSpread = root.GetChild("spread");
    for (XMLElement param = bulletSpread.GetChild("param"); param; param = param.GetNext("param"))
    {
        String name = param.GetAttribute("name");
        if      (name == "spread")   config_.spread.spread = param.GetFloat("value");
        else if (name == "count")    config_.spread.count = Max(2, param.GetInt("value"));
        else if (name == "cooldown") config_.spread.cooldown = param.GetFloat("value");
        else URHO3D_LOGERRORF("Unknown parameter mine \"%s\" while reading config file \"%s\"", name.CString(), configXML_->GetName().CString());
    }

    XMLElement mine = root.GetChild("mine");
    for (XMLElement param = mine.GetChild("param"); param; param = param.GetNext("param"))
    {
        String name = param.GetAttribute("name");
        if      (name == "ejectSpeed")    config_.mine.ejectSpeed = param.GetFloat("value");
        else if (name == "deceleration")  config_.mine.deceleration = param.GetFloat("value");
        else if (name == "life")          config_.mine.life = param.GetFloat("value");
        else if (name == "cooldown")      config_.mine.cooldown = param.GetFloat("value");
        else if (name == "initialOffset") config_.mine.initialOffset = param.GetFloat("value");
        else URHO3D_LOGERRORF("Unknown parameter mine \"%s\" while reading config file \"%s\"", name.CString(), configXML_->GetName().CString());
    }
}

// ----------------------------------------------------------------------------
bool WeaponSpawner::TryGetActionState()
{
    if (state_.NotNull())
    {
        UnsubscribeFromEvent(E_ACTIONWARP);
        UnsubscribeFromEvent(E_ACTIONUSEITEM);
        state_ = nullptr;
    }

    state_ = GetComponent<ActionState>();

    if (state_.NotNull())
    {
        SubscribeToEvent(state_, E_ACTIONWARP, URHO3D_HANDLER(WeaponSpawner, HandleActionWarp));
        SubscribeToEvent(state_, E_ACTIONUSEITEM, URHO3D_HANDLER(WeaponSpawner, HandleActionUseItem));
    }

    return state_.Expired();
}

// ----------------------------------------------------------------------------
void WeaponSpawner::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    float dt = eventData[P_TIMESTEP].GetFloat();

    if (state_.Expired() && TryGetActionState() == false)
        return;

    fireActionCooldown_ = Max(0.0, fireActionCooldown_ - dt);
    if (fireActionCooldown_ == 0.0 && state_->IsFiring())
    {
        fireActionCooldown_ = config_.phaser.cooldown;
        CreatePhaser();
    }
}

// ----------------------------------------------------------------------------
void WeaponSpawner::HandleActionWarp(StringHash eventType, VariantMap& eventData)
{
    URHO3D_LOGDEBUG("Warp action");
}

// ----------------------------------------------------------------------------
void WeaponSpawner::HandleActionUseItem(StringHash eventType, VariantMap& eventData)
{
    CreateMine();
}

// ----------------------------------------------------------------------------
void WeaponSpawner::HandleFileChanged(StringHash eventType, VariantMap& eventData)
{
    using namespace FileChanged;

    if (configXML_ && configXML_->GetName() == eventData[P_RESOURCENAME].GetString())
    {
        ParseConfig();
    }
}

}
