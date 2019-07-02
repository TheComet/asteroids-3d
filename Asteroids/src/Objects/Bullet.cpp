#include "Asteroids/Objects/Bullet.hpp"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
Bullet::Bullet(Context* context) :
    SurfaceObject(context),
    life_(1)
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Bullet, HandleUpdate));
}

// ----------------------------------------------------------------------------
Bullet* Bullet::Create(Scene* scene, Quaternion pivotRotation, float angle)
{
    ResourceCache* cache = scene->GetSubsystem<ResourceCache>();

    Node* pivotNode = scene->CreateChild("BulletPivot");
    Node* modelNode = pivotNode->CreateChild("ModelNode");
    modelNode->SetPosition(Vector3(0, 1, 0));
    modelNode->SetRotation(Quaternion(0, angle, 0));
    pivotNode->SetRotation(pivotRotation);

    StaticModel* bulletModel = modelNode->CreateComponent<StaticModel>();
    bulletModel->SetModel(cache->GetResource<Model>("Models/TestBullet.mdl"));
    bulletModel->SetMaterial(cache->GetResource<Material>("Materials/DefaultGrey.xml"));

    Bullet* bullet = modelNode->CreateComponent<Bullet>();
    bullet->SetLife(0.6);
    bullet->CalculateVelocity();
    bullet->UpdatePlanetHeight();

    return bullet;
}

// ----------------------------------------------------------------------------
void Bullet::Destroy(Bullet* bullet)
{
    bullet->GetNode()->GetParent()->Remove();
}

// ----------------------------------------------------------------------------
void Bullet::CalculateVelocity()
{
    float angle = node_->GetRotation().EulerAngles().y_;
    velocity_ = Vector2(Sin(angle), Cos(angle)) * 6000;
}

// ----------------------------------------------------------------------------
void Bullet::SetLife(float life)
{
    life_ = life;
}

// ----------------------------------------------------------------------------
void Bullet::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    float dt = eventData[P_TIMESTEP].GetFloat();

    UpdatePosition(velocity_, dt);
    UpdatePlanetHeight();

    life_ -= dt;
    if (life_ < 0)
    {
        Destroy(this);
    }
}

}
