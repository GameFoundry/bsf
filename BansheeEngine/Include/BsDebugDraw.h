#pragma once

#include "BsPrerequisites.h"
#include "CmModule.h"
#include "CmColor.h"
#include "CmAABox.h"

namespace BansheeEngine
{
	class BS_EXPORT DebugDraw : public CM::Module<DebugDraw>
	{
	public:
		DebugDraw();

		// TODO - Add a version that accepts 4 points
		void quad2D(const CM::Vector2& pos, const CM::Vector2& size, CM::UINT8* outVertices, CM::UINT8* outColors, 
			CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset, const CM::Color& color = CM::Color::White);

		// TODO - Need a version that accepts a camera otherwise they will draw on all cameras
		void drawQuad2D(const CM::Vector2& pos, const CM::Vector2& size, const CM::Color& color = CM::Color::White, float timeout = 0.0f);

		void render(CM::RenderQueue& renderQueue);

	private:
		CM::HMaterial mMaterial;
		CM::Vector<CM::HMesh>::type mMeshes;
	};
}