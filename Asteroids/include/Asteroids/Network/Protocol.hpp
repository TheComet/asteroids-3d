#pragma once

#include <stdint.h>

namespace Asteroids {

static const int MSG_CLIENT_SHIP_STATE = 0xA0;
static const int MSG_SERVER_SHIP_STATE = 0xA1;
static const int MSG_REGISTER_FAILED   = 0xA2;
static const int MSG_NETWORK_TIMER     = 0xA3;

enum MsgRegisterFailed
{
    USERNAME_TOO_LONG,
    USERNAME_EMPTY,
    USERNAME_ALREADY_TAKEN,
    USERNAME_BANNED
};

}
