#pragma once

#include "BsPrerequisites.h"
#include "CmModule.h"
#include "CmColor.h"
#include "CmAABox.h"

namespace BansheeEngine
{
	class BS_EXPORT DrawHelper : public CM::Module<DrawHelper>
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
		DrawHelper();

		/**
		 * @brief	TODO
		 *
		 * @param	pos				TODO
		 * @param	size			TODO
		 * @param	meshData		Mesh data that will be populated by this method.
		 * @param	vertexOffset	Number of vertices from buffer start to start writing at.
		 * @param	indexOffset 	Number of indices from buffer start to start writing at.
		 * 							
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector2 VES_POSITION
		 * 			  32bit index buffer
		 */
		void quad2D(const CM::Vector2& pos, const CM::Vector2& size, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset);

		/**
		 * @brief	TODO
		 *
		 * @param	pos				TODO
		 * @param	size			TODO
		 * @param	meshData		Mesh data that will be populated by this method.
		 * @param	vertexOffset	Number of vertices from buffer start to start writing at.
		 * @param	indexOffset 	Number of indices from buffer start to start writing at.
		 * 							
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector2 VES_POSITION
		 * 			  UINT32  VES_COLOR
		 * 			  32bit index buffer
		 */
		void line2D_Pixel(const CM::Vector2& a, const CM::Vector2& b, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset);

		/**
		 * @brief	TODO
		 *
		 * @param	pos				TODO
		 * @param	size			TODO
		 * @param	meshData		Mesh data that will be populated by this method.
		 * @param	vertexOffset	Number of vertices from buffer start to start writing at.
		 * @param	indexOffset 	Number of indices from buffer start to start writing at.
		 * 							
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector2 VES_POSITION
		 * 			  UINT32  VES_COLOR
		 * 			  32bit index buffer
		 */
		void line2D_AA(const CM::Vector2& a, const CM::Vector2& b, float width, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset);

		void drawQuad2D(const HCamera& camera, const CM::Vector2& pos, const CM::Vector2& size, const CM::Color& color = CM::Color::White, float timeout = 0.0f);

		void render(const HCamera& camera, CM::RenderQueue& renderQueue);

	private:
		CM::HMaterial mMaterial2D;

		CM::UnorderedMap<const CM::Viewport*, CM::Vector<DebugDrawCommand>::type>::type mCommandsPerViewport;

		// TODO - Add a version that accepts 4 points
		void quad2D(const CM::Vector2& pos, const CM::Vector2& size, CM::UINT8* outVertices, CM::UINT32 vertexOffset, 
			CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset);

		void line2D_Pixel(const CM::Vector2& a, const CM::Vector2& b, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
			CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset);

		void line2D_AA(const CM::Vector2& a, const CM::Vector2& b, float width, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
			CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset);

		void polygon2D_AA(const CM::Vector<CM::Vector2>::type& points, float width, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
			CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset);
	};
}