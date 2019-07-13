#pragma once

#include <stdint.h>

namespace Asteroids {

static const int MSG_CLIENT_SHIP_STATE = 0x17;
static const int MSG_SERVER_SHIP_STATE = 0x18;
static const int MSG_REGISTER_FAILED = 0x19;
static const int MSG_NETWORK_TIMER = 0x20;

enum MsgRegisterFailed
{
    USERNAME_TOO_LONG,
    USERNAME_EMPTY,
    USERNAME_ALREADY_TAKEN,
    USERNAME_BANNED
};

}
