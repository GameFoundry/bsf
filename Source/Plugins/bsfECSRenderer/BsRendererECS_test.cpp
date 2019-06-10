#include "Testing/BsTestSuite.h"
#include "bsfEnTT/fwd.h"
#include <entt/entt.hpp>
#include "bsfEnTT/RenderComponents/ERenderable.h"
#include "Importer/BsImporter.h"
#include "Resources/BsBuiltinResources.h"
#include "Material/BsMaterial.h"

#include "Scene/BsSceneObject.h"
#include "Components/BsCCamera.h"
#include "Components/BsCRenderable.h"
#include "Scene/BsSceneManager.h"
#include "bsfEnTT/Scene/Registry.h"
#include "CoreThread/BsCoreThread.h"


namespace bs::ecs {
	class ECSRenderableTestSuite : public bs::Test {};

	// TEST_F is required.
	TEST_F(ECSRenderableTestSuite, MakeRenderable) {
		Registry reg;
		EntityType ent = reg.create();
		auto& renderable = reg.assign<bs::ecs::ERenderable>(ent);

		// auto importOptions = MeshImportOptions::create();
		auto& importer = gImporter();
		HMesh mesh = importer.import<Mesh>("cube.obj");

		renderable.mMesh = mesh;
		ASSERT_TRUE(mesh->getCore());
		auto bounds = renderable.getBounds();
		ASSERT_GT(bounds.getSphere().getRadius(), 0);

		HShader shader = BuiltinResources::instance().getBuiltinShader(BuiltinShader::Standard);
		HMaterial material = Material::create(shader);

		renderable.mMaterials.push_back(material);
	}



	TEST_F(ECSRenderableTestSuite, MakeStandard) {

		HSceneObject sceneCameraSO = SceneObject::create("SceneCamera");
		HCamera sceneCamera = sceneCameraSO->addComponent<CCamera>();
		sceneCamera->setMain(true);
		sceneCameraSO->setPosition(Vector3(10.0f, 10.0f, 10.0f));
		sceneCameraSO->lookAt(Vector3(0, 0, 0));


		// Default Box
		HShader shader = gBuiltinResources().getBuiltinShader(BuiltinShader::Standard);
		HMaterial material = Material::create(shader);
		HMesh boxMesh = gBuiltinResources().getMesh(BuiltinMesh::Box);
		HSceneObject boxSO = SceneObject::create("Box");
		HRenderable boxRenderable = boxSO->addComponent<CRenderable>();
		boxRenderable->setMesh(boxMesh);
		boxRenderable->setMaterial(material);
		boxSO->setPosition(Vector3(0.0f, 0.0f, 0.0f));

		ASSERT_TRUE(SceneManager::isStarted());
		ASSERT_TRUE(ECSManager::isStarted());

    	Application::instance().runMainSteps(4);

    	auto reg = ECSManager::instance().getRegistry();
    	ASSERT_GT(reg->size(), 0);

    	bool forceRemoveAll = true;
    	SceneManager::instance().clearScene(forceRemoveAll);
	}
}
