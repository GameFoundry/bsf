#pragma once

#include "BsPrerequisites.h"
#include "BsDrawHelperTemplate.h"
#include "CmModule.h"
#include "CmColor.h"
#include "CmAABox.h"

namespace BansheeEngine
{
	class BS_EXPORT DrawHelper3D : public DrawHelperTemplate<CM::Vector3>, public CM::Module<DrawHelper3D>
	{
	public:
		DrawHelper3D();

		/**
		 * @brief	Fills the mesh data with vertices representing an axis aligned box.
		 *
		 * @param	aabox			Axis aligned box to get the mesh data for
		 * @param	color			Color of the line.
		 * @param	meshData		Mesh data that will be populated.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * 							
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector3 VES_POSITION
		 * 			  32bit index buffer
		 * 			  Enough space for 8 vertices and 36 indices
		 */
		void aabox(const CM::AABox& box, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset);

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
		 * 			  Vector3 VES_POSITION
		 * 			  UINT32  VES_COLOR
		 * 			  32bit index buffer
		 * 			  Enough space for 2 vertices and 2 indices
		 */
		void line_Pixel(const CM::Vector3& a, const CM::Vector3& b, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset);

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
		 * 			  Vector3 VES_POSITION
		 * 			  UINT32  VES_COLOR
		 * 			  32bit index buffer
		 *			  Enough space for 8 vertices and 30 indices
		 */
		void line_AA(const CM::Vector3& a, const CM::Vector3& b, float width, float borderWidth, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset);

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
		 * 			  Vector3 VES_POSITION
		 * 			  UINT32  VES_COLOR
		 * 			  32bit index buffer
		 * 			  Enough space for (numLines * 2) vertices and (numLines * 2) indices
		 */
		void lineList_Pixel(const CM::Vector<CM::Vector3>::type& linePoints, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset);

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
		 * 			  Vector3 VES_POSITION
		 * 			  UINT32  VES_COLOR
		 * 			  32bit index buffer
		 *			  Enough space for (numLines * 8) vertices and (numLines * 30) indices
		 */
		void lineList_AA(const CM::Vector<CM::Vector3>::type& linePoints, float width, float borderWidth, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset);

		void drawLine_Pixel(const HCamera& camera, const CM::Vector3& a, const CM::Vector3& b, const CM::Color& color = CM::Color::White, float timeout = 0.0f);
		void drawLine_AA(const HCamera& camera, const CM::Vector3& a, const CM::Vector3& b, float width, float borderWidth, 
			const CM::Color& color = CM::Color::White, float timeout = 0.0f);
		void drawLineList_Pixel(const HCamera& camera, const CM::Vector<CM::Vector3>::type& linePoints, const CM::Color& color = CM::Color::White, float timeout = 0.0f);
		void drawLineList_AA(const HCamera& camera, const CM::Vector<CM::Vector3>::type& linePoints, float width, float borderWidth, 
			const CM::Color& color = CM::Color::White, float timeout = 0.0f);

		void drawAABox(const HCamera& camera, const CM::AABox& box, const CM::Color& color = CM::Color::White, float timeout = 0.0f);

	private:
		CM::VertexDataDescPtr mVertexDesc;
		
		CM::Vector3 calcCenter(CM::UINT8* vertices, CM::UINT32 numVertices, CM::UINT32 vertexStride);

	protected:
		void line_AA(const CM::Vector3& a, const CM::Vector3& b, float width, float borderWidth, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
			CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset);

		void polygon_AA(const CM::Vector<CM::Vector3>::type& points, float borderWidth, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
			CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset);

		void aabox(const CM::AABox& box, CM::UINT8* outVertices, CM::UINT32 vertexOffset, CM::UINT32 vertexStride, 
			CM::UINT32* outIndices, CM::UINT32 indexOffset);
	};
}