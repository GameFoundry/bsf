#include "Testing/BsTestSuite.h"
#include "bsfEnTT/fwd.h"
#include <entt/entt.hpp>
#include "bsfEnTT/RenderComponents/ERenderable.h"
#include "Importer/BsImporter.h"



namespace bs::ecs {
	class RenderableTestSuite : public bs::Test {};

	// TEST_F is required.
	TEST_F(RenderableTestSuite, MakeRenderable) {
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
		HMesh m = renderable.getMesh();
	}
}