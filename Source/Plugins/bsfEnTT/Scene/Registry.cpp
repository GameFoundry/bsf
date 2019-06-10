#include "./Registry.h"
#include <entt/entt.hpp>

namespace bs::ecs {
  ECSManager::~ECSManager() {}

  ECSManager::ECSManager() :
    registry(std::make_unique<Registry>())
  {}
}
