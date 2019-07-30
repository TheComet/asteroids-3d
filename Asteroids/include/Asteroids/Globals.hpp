#pragma once

namespace Asteroids {

static const unsigned COLLISION_MASK_PLANET_TERRAIN  = 0x0001;
static const unsigned COLLISION_MASK_PLANET_WALLS    = 0x0002;
static const unsigned COLLISION_MASK_PLAYERS         = 0x0004;
static const unsigned COLLISION_MASK_PROJECTILES     = 0x0008;

static const float MAX_PLANET_RADIUS = 1000.0;

static const unsigned short DEFAULT_PORT = 6666;

}
