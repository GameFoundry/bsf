#pragma once

#include "BsPrerequisites.h"
#include "BsDrawHelperTemplate.h"
#include "CmModule.h"
#include "CmColor.h"
#include "CmAABox.h"

namespace BansheeEngine
{
	class BS_EXPORT DrawHelper2D : public DrawHelperTemplate<CM::Vector2>, public CM::Module<DrawHelper2D>
	{
	public:
		DrawHelper2D();

		/**
		 * @brief	Fills the mesh data with vertices representing a quad (2 triangles).
		 *
		 * @param	area			Area in which to draw the quad.
		 * @param	meshData		Mesh data that will be populated.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * 							
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector2 VES_POSITION
		 * 			  32bit index buffer
		 * 			  Enough space for 4 vertices and 6 indices
		 */
		void quad(const CM::FRect& area, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset);

		/**
		 * @brief	Fills the mesh data with vertices representing a per-pixel line.
		 *
		 * @param	a				Start point of the line.
		 * @param	b				End point of the line.
		 * @param	color			Color of the line.
		 * @param	meshData		Mesh data that will be populated.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * 							
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector2 VES_POSITION
		 * 			  UINT32  VES_COLOR
		 * 			  32bit index buffer
		 * 			  Enough space for 2 vertices and 2 indices
		 */
		void line_Pixel(const CM::Vector2& a, const CM::Vector2& b, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset);

		/**
		 * @brief	Fills the mesh data with vertices representing an anti-aliased line of specific width. Antialiasing is done using alpha blending.
		 *
		 * @param	a				Start point of the line.
		 * @param	b				End point of the line.
		 * @param	width			Width of the line.
		 * @param	borderWidth		Width of the anti-aliased border.
		 * @param	color			Color of the line.
		 * @param	meshData		Mesh data that will be populated by this method.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * 							
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector2 VES_POSITION
		 * 			  UINT32  VES_COLOR
		 * 			  32bit index buffer
		 *			  Enough space for 8 vertices and 30 indices
		 */
		void line_AA(const CM::Vector2& a, const CM::Vector2& b, float width, float borderWidth, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset);

		/**
		 * @brief	Fills the mesh data with vertices representing per-pixel lines.
		 *
		 * @param	linePoints		A list of start and end points for the lines. Must be a multiple of 2.
		 * @param	color			Color of the line.
		 * @param	meshData		Mesh data that will be populated.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * 							
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector2 VES_POSITION
		 * 			  UINT32  VES_COLOR
		 * 			  32bit index buffer
		 * 			  Enough space for (numLines * 2) vertices and (numLines * 2) indices
		 */
		void lineList_Pixel(const CM::Vector<CM::Vector2>::type& linePoints, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset);

		/**
		 * @brief	Fills the mesh data with vertices representing anti-aliased lines of specific width. Antialiasing is done using alpha blending.
		 *
		 * @param	linePoints		A list of start and end points for the lines. Must be a multiple of 2.
		 * @param	width			Width of the line.
		 * @param	borderWidth		Width of the anti-aliased border.
		 * @param	color			Color of the line.
		 * @param	meshData		Mesh data that will be populated by this method.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * 							
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector2 VES_POSITION
		 * 			  UINT32  VES_COLOR
		 * 			  32bit index buffer
		 *			  Enough space for (numLines * 8) vertices and (numLines * 30) indices
		 */
		void lineList_AA(const CM::Vector<CM::Vector2>::type& linePoints, float width, float borderWidth, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset);

		void drawQuad(const HCamera& camera, const CM::FRect& area, const CM::Color& color = CM::Color::White, DebugDrawCoordType coordType = DebugDrawCoordType::Pixel, float timeout = 0.0f);
		void drawLine_Pixel(const HCamera& camera, const CM::Vector2& a, const CM::Vector2& b, const CM::Color& color = CM::Color::White, 
			DebugDrawCoordType coordType = DebugDrawCoordType::Pixel, float timeout = 0.0f);
		void drawLine_AA(const HCamera& camera, const CM::Vector2& a, const CM::Vector2& b, float width, float borderWidth, 
			const CM::Color& color = CM::Color::White, DebugDrawCoordType coordType = DebugDrawCoordType::Pixel, float timeout = 0.0f);
		void drawLineList_Pixel(const HCamera& camera, const CM::Vector<CM::Vector2>::type& linePoints, const CM::Color& color = CM::Color::White, 
			DebugDrawCoordType coordType = DebugDrawCoordType::Pixel, float timeout = 0.0f);
		void drawLineList_AA(const HCamera& camera, const CM::Vector<CM::Vector2>::type& linePoints, float width, float borderWidth, 
			const CM::Color& color = CM::Color::White, DebugDrawCoordType coordType = DebugDrawCoordType::Pixel, float timeout = 0.0f);

	private:
		DebugDraw2DClipSpaceMatInfo mMaterial2DClipSpace;

		CM::VertexDataDescPtr mVertexDesc;

		CM::FRect normalizedCoordToClipSpace(const CM::FRect& area) const;
		CM::Vector2 normalizedCoordToClipSpace(const CM::Vector2& pos) const;

	protected:
		void line_AA(const CM::Vector2& a, const CM::Vector2& b, float width, float borderWidth, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
			CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset);

		void polygon_AA(const CM::Vector<CM::Vector2>::type& points, float borderWidth, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
			CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset);
	};
}