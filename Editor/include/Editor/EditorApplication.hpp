#pragma once

#include <Urho3D/Engine/Application.h>

namespace Urho3D {
    class ScriptFile;
}

namespace Asteroids {

class EditorApplication : public Urho3D::Application
{
public:
    EditorApplication(Urho3D::Context* context);

    virtual void Setup() override;
    virtual void Start() override;
    virtual void Stop() override;

private:
    Urho3D::SharedPtr<Urho3D::ScriptFile> script_;
};

}

