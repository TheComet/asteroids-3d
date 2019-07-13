#pragma once

#include "Asteroids/Config.hpp"
#include <Urho3D/Scene/Component.h>

namespace Asteroids {

class ASTEROIDS_PUBLIC_API ActionState : public Urho3D::Component
{
    URHO3D_OBJECT(ActionState, Urho3D::Component);

    typedef uint16_t Data;

public:
    ActionState(Urho3D::Context* context);

    static void RegisterObject(Urho3D::Context* context);

    Data GetState() const;
    void SetState(Data data);

    float GetLeft() const;
    void SetLeft(float value);

    float GetRight() const;
    void SetRight(float value);

    bool IsFiring() const;
    void SetFiring(bool enable);

    bool IsThrusting() const;
    void SetThrusting(bool enable);

    void SetWarp(bool enable);
    void SetUseItem(bool enable);

private:

    union InputState
    {
        struct
        {
            unsigned left    : 6;
            unsigned right   : 6;
            unsigned fire    : 1;
            unsigned thrust  : 1;
            unsigned warp    : 1;
            unsigned useItem : 1;
        } data;
        uint16_t u16;
    } inputState_;
};

}
