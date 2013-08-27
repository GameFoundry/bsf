#pragma once

#include "BsPrerequisites.h"
#include "CmModule.h"
#include "CmColor.h"
#include "CmAABox.h"

namespace BansheeEngine
{
	enum class CoordType
	{
		Pixel,
		Normalized
	};

	enum class DrawStyle
	{
		Fill,
		Border,
		FillAndBorder
	};

	class BS_EXPORT DrawHelper : public CM::Module<DrawHelper>
	{
		enum class DebugDrawType
		{
			ClipSpace,
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
		void quad2D(const CM::FRect& area, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset);

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
		void line2D_Pixel(const CM::Vector2& a, const CM::Vector2& b, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset);

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
		void line2D_AA(const CM::Vector2& a, const CM::Vector2& b, float width, float borderWidth, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset);

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
		void lineList2D_Pixel(const CM::Vector<CM::Vector2>::type& linePoints, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset);

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
		void lineList2D_AA(const CM::Vector<CM::Vector2>::type& linePoints, float width, float borderWidth, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset);

		void drawQuad2D(const HCamera& camera, const CM::FRect& area, const CM::Color& color = CM::Color::White, CoordType coordType = CoordType::Pixel, float timeout = 0.0f);
		void drawLine2D_Pixel(const HCamera& camera, const CM::Vector2& a, const CM::Vector2& b, const CM::Color& color = CM::Color::White, 
			CoordType coordType = CoordType::Pixel, float timeout = 0.0f);
		void drawLine2D_AA(const HCamera& camera, const CM::Vector2& a, const CM::Vector2& b, float width, float borderWidth, 
			const CM::Color& color = CM::Color::White, CoordType coordType = CoordType::Pixel, float timeout = 0.0f);
		void drawLineList2D_Pixel(const HCamera& camera, const CM::Vector<CM::Vector2>::type& linePoints, const CM::Color& color = CM::Color::White, 
			CoordType coordType = CoordType::Pixel, float timeout = 0.0f);
		void drawLineList2D_AA(const HCamera& camera, const CM::Vector<CM::Vector2>::type& linePoints, float width, float borderWidth, 
			const CM::Color& color = CM::Color::White, CoordType coordType = CoordType::Pixel, float timeout = 0.0f);

		void render(const HCamera& camera, CM::RenderQueue& renderQueue);

	private:
		CM::HMaterial mMaterial2DClipSpace;

		CM::UnorderedMap<const CM::Viewport*, CM::Vector<DebugDrawCommand>::type>::type mCommandsPerViewport;

		void line2D_Pixel(const CM::Vector2& a, const CM::Vector2& b, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
			CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset);

		void line2D_AA(const CM::Vector2& a, const CM::Vector2& b, float width, float borderWidth, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
			CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset);

		void polygon2D_AA(const CM::Vector<CM::Vector2>::type& points, float borderWidth, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
			CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset);

		void polygonFill2D_Pixel(const CM::Vector<CM::Vector2>::type& points, CM::UINT8* outVertices, 
			CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset);

		void polygonBorder2D_Pixel(const CM::Vector<CM::Vector2>::type& points, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
			CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset);

		CM::FRect normalizedCoordToClipSpace(const CM::FRect& area) const;
		CM::Vector2 normalizedCoordToClipSpace(const CM::Vector2& pos) const;
	};
}