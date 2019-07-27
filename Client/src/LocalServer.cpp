#include "Client/LocalServer.hpp"
#include "Asteroids/Util/Process.hpp"

#include <Urho3D/Core/Context.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
LocalServer::LocalServer(Context* context) :
    Object(context),
    serverProcess_(new Process)
{
}

// ----------------------------------------------------------------------------
bool LocalServer::Start(int port)
{
    StringVector args;
#if defined(_WIN32)
#   if defined(DEBUG)
    args.Push("asteroids-server_d.exe");
#   else
    args.Push("asteroids-server.exe");
#   endif
#else
    args.Push("asteroids-server");
#endif
    args.Push("--port");
    args.Push(String(port));
    return serverProcess_->Open(args);
}

// ----------------------------------------------------------------------------
void LocalServer::Stop()
{
    serverProcess_->Close();
}

// ----------------------------------------------------------------------------
void LocalServer::ForceStop()
{
    serverProcess_->Terminate();
}

// ----------------------------------------------------------------------------
bool LocalServer::IsRunning() const
{
    return false;
}

}
