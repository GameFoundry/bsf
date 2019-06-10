#pragma once

#include "BsPrerequisites.h"
#include "Utility/BsModule.h"
#include "bsfEnTT/fwd.h"

namespace bs::ecs {

  class BS_CORE_EXPORT ECSManager : public Module<ECSManager> {
    std::unique_ptr<Registry> registry;

  public:
    ECSManager();
    ~ECSManager();
    void onStartUp() override;
    void onShutDown() override;
    Registry* getRegistry() { return registry.get(); }
  };
}  // namespace bs
