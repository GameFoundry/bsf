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
#include "BsRendererRenderable.h"
#include "Image/BsTexture.h"
#include "Components/BsCDecal.h"


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


	void addBox() {
		HShader shader = gBuiltinResources().getBuiltinShader(BuiltinShader::Standard);
		HMaterial material = Material::create(shader);
		HMesh boxMesh = gBuiltinResources().getMesh(BuiltinMesh::Box);
		HSceneObject boxSO = SceneObject::create("Box");
		HRenderable boxRenderable = boxSO->addComponent<CRenderable>();
		boxRenderable->setMesh(boxMesh);
		boxRenderable->setMaterial(material);
		boxSO->setPosition(Vector3(0.0f, 0.0f, 0.0f));
	}

	void screenShot() {
		TEXTURE_DESC colorDesc;
		colorDesc.type = TEX_TYPE_2D;
		colorDesc.width = 1920 / 2;
		colorDesc.height = 1080 / 2;
		// colorDesc.format = PF_RGB32F;
		colorDesc.usage = TU_RENDERTARGET | TU_CPUREADABLE;
		HTexture color = Texture::create(colorDesc);

		RENDER_TEXTURE_DESC desc;
		desc.colorSurfaces[0].texture = color;
		desc.colorSurfaces[0].face = 0;
		desc.colorSurfaces[0].mipLevel = 0;
		SPtr<RenderTexture> renderTexture = RenderTexture::create(desc);

		// auto cam = Camera::create(renderTexture);
		// HSceneObject cameraSO = SceneObject::create("Camera");
		// HCamera camera = cameraSO->addComponent<CCamera>();
		SPtr<Camera> cam = gSceneManager().getMainCamera();
		auto origTarget = cam->getViewport()->getTarget();
		cam->getViewport()->setTarget(renderTexture);
    	Application::instance().runMainSteps(1);
    	cam->getViewport()->setTarget(origTarget);

    	ASSERT_TRUE(renderTexture->getColorTexture(0));
    	ASSERT_TRUE(renderTexture->getColorTexture(0).isLoaded(false));

    	// unfotunately there's no utility for
    	// taking a texture and writing it out as an image file.
    	// it will be a very useful utility for future.

	}

	void addDecal() {
		HShader decalShader = gBuiltinResources().getBuiltinShader(BuiltinShader::Decal);
		HMaterial decalMaterial = Material::create(decalShader);
		HSceneObject decalSO = SceneObject::create("Decal");
		decalSO->setPosition(Vector3(0.0f, 6.0f, 0.0f));
		decalSO->lookAt(Vector3(0.0f, 0.0f, 0.0f));
		HDecal decal = decalSO->addComponent<CDecal>();
		decal->setMaterial(decalMaterial);
	}


	TEST_F(ECSRenderableTestSuite, MakeStandard) {

		HSceneObject sceneCameraSO = SceneObject::create("SceneCamera");
		HCamera sceneCamera = sceneCameraSO->addComponent<CCamera>();
		sceneCamera->setMain(true);
		sceneCameraSO->setPosition(Vector3(10.0f, 10.0f, 10.0f));
		sceneCameraSO->lookAt(Vector3(0, 0, 0));


		// Default Box
		addBox();

    	Application::instance().runMainSteps(4);

    	auto reg = gRegistry();
    	ASSERT_GT(reg->size(), 0);
    	ASSERT_GT(reg->size<bs::ct::RendererRenderable>(), 0);

    	screenShot();

    	addDecal();

    	Application::instance().runMainSteps(4);

    	bool forceRemoveAll = true;
    	SceneManager::instance().clearScene(forceRemoveAll);
	}

}
