#pragma once

#include <Urho3D/UI/UIElement.h>

namespace Asteroids {

/*!
 * Helper function for getting a child UI element in a typesafe way.
 */
template <class T>
T* GetUIChild(const Urho3D::UIElement* element, const Urho3D::String& name)
{
    Urho3D::UIElement* child = element->GetChild(name, true);
    if (child == nullptr)
        return nullptr;
    if (child->GetType() != T::GetTypeStatic())
        return nullptr;
    return static_cast<T*>(child);
}

}
