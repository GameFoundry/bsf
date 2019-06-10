#include "./Registry.h"
#include <entt/entt.hpp>

namespace bs::ecs {
  ECSManager::~ECSManager() {}


  void ECSManager::onStartUp() {
  	std::cout << "ECS MANA UP" << std::endl;
  }

  void ECSManager::onShutDown() {
  	std::cout << "ECS DOWN " << std::endl;
  }

  ECSManager::ECSManager() :
    registry(std::make_unique<Registry>())
  {}
}
