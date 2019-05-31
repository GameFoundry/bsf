#pragma once

#include "fwd.h"

// utility methods to wrap utility actor.

namespace ecs {

void addChildToParent(Registry& reg, EntityType parent, EntityType child);


} // namespace ecs