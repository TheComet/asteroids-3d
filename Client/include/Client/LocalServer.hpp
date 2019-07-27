#pragma once

#include "Asteroids/Config.hpp"
#include <Urho3D/Core/Object.h>

namespace Asteroids {

class Process;

class LocalServer : public Urho3D::Object
{
    URHO3D_OBJECT(LocalServer, Urho3D::Object)

public:
    LocalServer(Urho3D::Context* context);

    bool Start(int port);
    void Stop();
    void ForceStop();
    bool IsRunning() const;

private:
    Urho3D::UniquePtr<Process> serverProcess_;
};

}
