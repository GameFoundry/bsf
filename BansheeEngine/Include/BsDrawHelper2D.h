#pragma once

#include "BsPrerequisites.h"
#include "BsDrawHelperTemplate.h"
#include "CmModule.h"
#include "CmColor.h"
#include "CmAABox.h"

namespace BansheeEngine
{
	class BS_EXPORT DrawHelper2D : public DrawHelperTemplate<Vector2>, public Module<DrawHelper2D>
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
		void quad(const RectF& area, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

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
		void line_Pixel(const Vector2& a, const Vector2& b, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

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
		void line_AA(const Vector2& a, const Vector2& b, float width, float borderWidth, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

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
		void lineList_Pixel(const Vector<Vector2>::type& linePoints, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

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
		void lineList_AA(const Vector<Vector2>::type& linePoints, float width, float borderWidth, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		void drawQuad(const HCamera& camera, const RectF& area, const Color& color = Color::White, DebugDrawCoordType coordType = DebugDrawCoordType::Pixel, float timeout = 0.0f);
		void drawLine_Pixel(const HCamera& camera, const Vector2& a, const Vector2& b, const Color& color = Color::White, 
			DebugDrawCoordType coordType = DebugDrawCoordType::Pixel, float timeout = 0.0f);
		void drawLine_AA(const HCamera& camera, const Vector2& a, const Vector2& b, float width, float borderWidth, 
			const Color& color = Color::White, DebugDrawCoordType coordType = DebugDrawCoordType::Pixel, float timeout = 0.0f);
		void drawLineList_Pixel(const HCamera& camera, const Vector<Vector2>::type& linePoints, const Color& color = Color::White, 
			DebugDrawCoordType coordType = DebugDrawCoordType::Pixel, float timeout = 0.0f);
		void drawLineList_AA(const HCamera& camera, const Vector<Vector2>::type& linePoints, float width, float borderWidth, 
			const Color& color = Color::White, DebugDrawCoordType coordType = DebugDrawCoordType::Pixel, float timeout = 0.0f);

	private:
		DebugDraw2DClipSpaceMatInfo mMaterial2DClipSpace;

		VertexDataDescPtr mVertexDesc;

		RectF normalizedCoordToClipSpace(const RectF& area) const;
		Vector2 normalizedCoordToClipSpace(const Vector2& pos) const;

	protected:
		void line_AA(const Vector2& a, const Vector2& b, float width, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors, 
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset);

		void polygon_AA(const Vector<Vector2>::type& points, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors, 
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset);
	};
}