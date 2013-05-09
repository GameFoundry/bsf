#pragma once

#include "BsPrerequisites.h"
#include "CmDeferredRenderContextFwd.h"
#include "CmModule.h"
#include "CmColor.h"
#include "CmAxisAlignedBox.h"

namespace BansheeEngine
{
	/**
	 * @brief	Methods you can use for debugging. Quite inefficient way of rendering though,
	 * 			so use them sparingly.
	 * 			
	 *			TODO - NOT THREAD SAFE
	 */
	class BS_EXPORT DebugDraw : public CM::Module<DebugDraw>
	{
		enum class DebugDrawType
		{
			Line, Triangle
		};

		struct DebugDrawCommand
		{
			DebugDrawType type;
			float timeEnds;

			UINT32* indices;
			UINT8* vertices;
			UINT32 numElements;
		};

	public:
		DebugDraw();

		void draw2DLine(const CM::Vector2&a, const CM::Vector2& b, const CM::Color& color = CM::Color::Green, float timeout = 0.0f);
		//void drawAABB(const AxisAlignedBox& aab, const Color& color = Color::Green, float timeout = 0.0f);
		//void drawSphere(const Vector3& center, float radius, const Color& color = Color::Green, float timeout = 0.0f);

		void render(const Camera* camera, CM::RenderContext& renderContext);

	private:
		CM::HMesh mTriangleMesh;
		CM::HMesh mLineMesh;

		CM::HMaterial mTriangleMaterial;
		CM::HMaterial mLineMaterial;

		std::vector<DebugDrawCommand> mCommands;

		static const int VertexSize = 16;

		void updateMeshes();
	};
}