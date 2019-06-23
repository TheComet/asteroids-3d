#include "Server/SignalHandler.hpp"
#include "Server/signals.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/Log.h>

using namespace Urho3D;

namespace Asteroids {

// ----------------------------------------------------------------------------
SignalHandler::SignalHandler(Context* context) :
    Object(context)
{
    signals_register();
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(SignalHandler, HandleUpdate));
}

// ----------------------------------------------------------------------------
void SignalHandler::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    if (signals_exit_requested())
    {
        URHO3D_LOGINFO("Signal caught, sending exit request event");
        SendEvent(E_EXITREQUESTED);
    }
}

}
