#pragma once

#include "BsPrerequisites.h"
#include "Utility/BsModule.h"
#include "bsfEnTT/fwd.h"

namespace bs::ecs {

  void startUp();
  void shutDown();
  Registry* gRegistry();
}  // namespace bs
