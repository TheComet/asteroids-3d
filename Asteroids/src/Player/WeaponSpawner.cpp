#include "Asteroids/AsteroidsLib.hpp"
#include "Asteroids/Objects/BulletController.hpp"
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
void WeaponSpawner::CreateBullet(float angleOffset)
{
    // Load bullet prefab
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Node* bullet = GetScene()->CreateChild();
    XMLFile* bulletXML = cache->GetResource<XMLFile>("Prefabs/Bullet.xml");
    bullet->LoadXML(bulletXML->GetRoot());

    // Calculate the effective bullet direction, which is a combination of the
    // player's angle and player's speed
    ShipController* shipController = GetComponent<ShipController>();
    Vector2 bulletStandingVelocity(Sin(shipController->GetAngle() + angleOffset) * config_.bullet.speed, Cos(shipController->GetAngle() + angleOffset) * config_.bullet.speed);
    Vector2 bulletVelocity = bulletStandingVelocity + shipController->GetVelocity();

    // Set up bullet controller with the correct speed/life parameters.
    // Ship controller should always exist if weapon spawner exists.
    BulletController* bulletController = bullet->GetChild("Bullet")->GetComponent<BulletController>();
    bulletController->SetLife(config_.bullet.life);
    bulletController->SetVelocity(bulletVelocity);

    // Set initial bullet location to the tip of the player's ship
    // Note: Have to update planet height before moving the bullet, as
    // UpdatePosition takes into account the planet's radius
    bullet->SetRotation(node_->GetParent()->GetRotation());
    bulletController->UpdatePlanetHeight();
    bulletController->UpdatePosition(bulletController->GetVelocity().Normalized(), config_.bullet.initialOffset);
}

// ----------------------------------------------------------------------------
void WeaponSpawner::CreateBulletSpread()
{
    float angle = -config_.bulletSpread.spread / 2;
    float incr = config_.bulletSpread.spread / (config_.bulletSpread.count - 1);
    for (int i = 0; i != config_.bulletSpread.count; ++i, angle += incr)
        CreateBullet(angle);
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
    XMLElement bullet = root.GetChild("bullet");
    for (XMLElement param = bullet.GetChild("param"); param; param = param.GetNext("param"))
    {
        String name = param.GetAttribute("name");
        if      (name == "speed")         config_.bullet.speed = param.GetFloat("value");
        else if (name == "life")          config_.bullet.life = param.GetFloat("value");
        else if (name == "cooldown")      config_.bullet.cooldown = param.GetFloat("value");
        else if (name == "initialOffset") config_.bullet.initialOffset = param.GetFloat("value");
        else
        {
            URHO3D_LOGERRORF("Unknown parameter bullet \"%s\" while reading config file \"%s\"", name.CString(), configXML_->GetName().CString());
        }
    }

    XMLElement bulletSpread = root.GetChild("bulletspread");
    for (XMLElement param = bulletSpread.GetChild("param"); param; param = param.GetNext("param"))
    {
        String name = param.GetAttribute("name");
        if      (name == "spread")   config_.bulletSpread.spread = param.GetFloat("value");
        else if (name == "count")    config_.bulletSpread.count = Max(2, param.GetInt("value"));
        else if (name == "cooldown") config_.bulletSpread.cooldown = param.GetFloat("value");
        else
        {
            URHO3D_LOGERRORF("Unknown parameter mine \"%s\" while reading config file \"%s\"", name.CString(), configXML_->GetName().CString());
        }
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
        else
        {
            URHO3D_LOGERRORF("Unknown parameter mine \"%s\" while reading config file \"%s\"", name.CString(), configXML_->GetName().CString());
        }
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
        fireActionCooldown_ = config_.bulletSpread.cooldown;
        CreateBulletSpread();
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
