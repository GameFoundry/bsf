#include "./Registry.h"
#include <entt/entt.hpp>

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


  ECSManager::~ECSManager() {}

  ECSManager::ECSManager() :
    registry(std::make_unique<Registry>())
  {}

  void shutDown() {
  	ECSManager::shutDown();
  }
  void startUp() {
  	ECSManager::startUp();
  }

  void ECSManager::onStartUp() {
  	// std::cout << "ECS MANA UP" << std::endl;
  }

  void ECSManager::onShutDown() {
  	// std::cout << "ECS DOWN " << std::endl;
  }

  Registry* gRegistry() {
  	return ECSManager::instance().getRegistry();
  }

}
