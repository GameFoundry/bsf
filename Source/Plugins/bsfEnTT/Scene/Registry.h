#pragma once

#include "BsPrerequisites.h"
#include "Utility/BsModule.h"
#include <fwd.h>

namespace bs::ecs {

  class BS_EXPORT ECSManager : public Module<ECSManager> {
    std::unique_ptr<Registry> registry;

  public:
    ECSManager();
    ~ECSManager();
    Registry* getRegistry() { return registry.get(); }
  };
}  // namespace bs
