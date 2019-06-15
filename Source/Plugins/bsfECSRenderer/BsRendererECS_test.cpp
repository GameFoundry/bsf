#include "Testing/BsTestSuite.h"
#include "bsfEnTT/fwd.h"
#include <entt/entt.hpp>
#include "bsfEnTT/RenderComponents/ERenderable.h"
#include "Importer/BsImporter.h"
#include "Resources/BsBuiltinResources.h"
#include "Material/BsMaterial.h"

#include "Scene/BsSceneObject.h"
#include "Components/BsCLight.h"
#include "Components/BsCCamera.h"
#include "Components/BsCRenderable.h"
#include "Components/BsCParticleSystem.h"
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


HSceneObject addBox() {
	HShader shader = gBuiltinResources().getBuiltinShader(BuiltinShader::Standard);
	HMaterial material = Material::create(shader);
	HMesh boxMesh = gBuiltinResources().getMesh(BuiltinMesh::Box);
	HSceneObject boxSO = SceneObject::create("Box");
	HRenderable boxRenderable = boxSO->addComponent<CRenderable>();
	boxRenderable->setMesh(boxMesh);
	boxRenderable->setMaterial(material);
	boxSO->setPosition(Vector3(0.0f, 0.5f, 0.5f));
	return boxSO;
}

HSceneObject addPlane() {

	HShader shader = gBuiltinResources().getBuiltinShader(BuiltinShader::Standard);
	HMaterial material = Material::create(shader);
	HMesh planeMesh = gBuiltinResources().getMesh(BuiltinMesh::Quad);
	HSceneObject planeSO = SceneObject::create("Quad");
	HRenderable boxRenderable = planeSO->addComponent<CRenderable>();
	boxRenderable->setMesh(planeMesh);
	boxRenderable->setMaterial(material);
	planeSO->setPosition(Vector3(0.0f, 0.0f, 0.0f));
	planeSO->setScale(Vector3(10.0f, 1.0f, 10.0f));
	return planeSO;
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
	decalMaterial->setTexture("gAlbedoTex", gBuiltinResources().getTexture(BuiltinTexture::Black));
	HSceneObject decalSO = SceneObject::create("Decal");
	decalSO->setPosition(Vector3(0.0f, 6.0f, 0.0f));
	decalSO->lookAt(Vector3(0.0f, 0.0f, 1.0f));
	HDecal decal = decalSO->addComponent<CDecal>();
	decal->setMaterial(decalMaterial);
	decalMaterial->setVariation(ShaderVariation(
		{
			// Use the default, transparent blend mode that uses traditional PBR textures to project. Normally no need
			// to set the default explicitly but it's done here for example purposes. See the manual for all available
			// modes
			ShaderVariation::Param("BLEND_MODE", 0)
		})
	);

}

void addParticles() {
	// Create a scene object and add a particle system component
	HSceneObject particleSystemSO = SceneObject::create("ParticleSystem");
	HParticleSystem particleSystem = particleSystemSO->addComponent<CParticleSystem>();
	particleSystemSO->setPosition(Vector3(0.f, 0.f, 0.f));

	// Create a material to use for rendering the particles
	HShader shader = gBuiltinResources().getBuiltinShader(BuiltinShader::ParticlesUnlit);
	HTexture texture = gBuiltinResources().getTexture(BuiltinTexture::White);

	HMaterial particleMaterial = Material::create(shader);
	particleMaterial->setTexture("gTexture", texture);

	// Set the material to be used by the particle system
	ParticleSystemSettings psSettings;
	psSettings.material = particleMaterial;

	particleSystem->setSettings(psSettings);

	// Add an emitter that emits particles on the surface of a sphere
	SPtr<ParticleEmitter> emitter = bs_shared_ptr_new<ParticleEmitter>();

	PARTICLE_SPHERE_SHAPE_DESC sphereShape;
	sphereShape.radius = 1.f;
	emitter->setShape(ParticleEmitterSphereShape::create(sphereShape));
	emitter->setEmissionRate(50.0f);
	emitter->setInitialLifetime(10.0f);
	emitter->setInitialSize(0.1f);
	emitter->setInitialSpeed(5.0f);
	particleSystem->setEmitters({emitter});
}

HSceneObject addDirectionalLight() {
	HSceneObject lightSO = SceneObject::create("Light");
	HLight light = lightSO->addComponent<CLight>();
	light->setType(LightType::Directional);
	lightSO->lookAt(Vector3(-1, -0.5, 0));
	light->setCastsShadow(true);
	light->setIntensity(10);
	return lightSO;
}

HSceneObject addRadialLight() {
	HSceneObject lightSO = SceneObject::create("Light");
	HLight light = lightSO->addComponent<CLight>();
	light->setType(LightType::Radial);
	lightSO->setPosition(Vector3(0, 0.5, 2.0));
	light->setSourceRadius(1.0);
	light->setColor(Color::Red);
	light->setCastsShadow(true);
	light->setIntensity(60);
	return lightSO;
}

HSceneObject addSpotLight() {
	HSceneObject lightSO = SceneObject::create("Light");
	HLight light = lightSO->addComponent<CLight>();
	light->setType(LightType::Spot);
	lightSO->setPosition(Vector3(0, 5, 0.0));
	lightSO->lookAt(Vector3(0, 0, 0));
	light->setSourceRadius(2.0);
	light->setSpotAngle(Degree(10));
	light->setColor(Color::Green);
	light->setCastsShadow(true);
	light->setIntensity(40);
	return lightSO;
}

TEST_F(ECSRenderableTestSuite, MakeStandard) {

	HSceneObject sceneCameraSO = SceneObject::create("SceneCamera");
	HCamera sceneCamera = sceneCameraSO->addComponent<CCamera>();
	sceneCamera->setMain(true);
	sceneCameraSO->setPosition(Vector3(10.0f, 10.0f, 10.0f));
	sceneCameraSO->lookAt(Vector3(0, 0, 0));


	// Default Box
	auto box = addBox();

	Application::instance().runMainSteps(4);

	// auto reg = gRegistry();
	// ASSERT_GT(reg->size(), 0);
	// ASSERT_GT(reg->size<bs::ct::RendererRenderable>(), 0);

	screenShot();


	auto plane = addPlane();
	addDecal();
	Application::instance().runMainLoop();
	Application::instance().runMainSteps(4);

	box->destroy();
	plane->destroy();
	addParticles();

	Application::instance().runMainSteps(4);

	bool forceRemoveAll = true;
	SceneManager::instance().clearScene(forceRemoveAll);
}

TEST_F(ECSRenderableTestSuite, TestLighting) {

	HSceneObject sceneCameraSO = SceneObject::create("SceneCamera");
	HCamera sceneCamera = sceneCameraSO->addComponent<CCamera>();
	sceneCamera->setMain(true);
	sceneCameraSO->setPosition(Vector3(10.0f, 10.0f, 10.0f));
	sceneCameraSO->lookAt(Vector3(0, 0, 0));

	// Default Box
	auto box = addBox();
	auto plane = addPlane();
	auto dirLight = addDirectionalLight();

	Application::instance().runMainSteps(4);

	// dirLight->destroy();
	addRadialLight();
	addSpotLight();

	Application::instance().runMainLoop();

	bool forceRemoveAll = true;
	SceneManager::instance().clearScene(forceRemoveAll);
}


}  // namespace bs::ecs
