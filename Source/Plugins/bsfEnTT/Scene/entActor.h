#pragma once

#include "fwd.h"

// utility methods to wrap utility actor.

namespace bs::ecs {

// tag for disabling entities from being updated.
struct Inactive {};
/**
 * Determines the mobility of the actor. This is used primarily as a performance hint to engine systems. Objects
 * with more restricted mobility will result in higher performance. Any transform changes to immobile actors will
 * be ignored. By default actor's mobility is unrestricted.
 */
struct TransformLocked {};

void addChildToParent(Registry& reg, EntityType parent, EntityType child);

} // namespace bs::ecs