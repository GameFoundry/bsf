#pragma once

#include "BsPrerequisites.h"
#include "CmModule.h"
#include "CmColor.h"
#include "CmAABox.h"

namespace BansheeEngine
{
	class BS_EXPORT DebugDraw : public CM::Module<DebugDraw>
	{
		enum class DebugDrawType
		{
			ScreenSpace,
			WorldSpace
		};

		struct DebugDrawCommand
		{
			CM::HMesh mesh;
			CM::HMaterial material;
			DebugDrawType type;
			CM::Vector3 worldCenter;
			float endTime;
		};

	public:
		DebugDraw();

		// TODO - Add a version that accepts 4 points
		void quad2D(const CM::Vector2& pos, const CM::Vector2& size, CM::UINT8* outVertices, CM::UINT8* outColors, 
			CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset, const CM::Color& color = CM::Color::White);

		void drawQuad2D(const HCamera& camera, const CM::Vector2& pos, const CM::Vector2& size, const CM::Color& color = CM::Color::White, float timeout = 0.0f);

		void render(const HCamera& camera, CM::RenderQueue& renderQueue);

	private:
		CM::HMaterial mMaterial2D;

		CM::UnorderedMap<const CM::Viewport*, CM::Vector<DebugDrawCommand>::type>::type mCommandsPerViewport;
	};
}