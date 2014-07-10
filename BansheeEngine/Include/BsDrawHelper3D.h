//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisites.h"
#include "BsDrawHelperTemplate.h"
#include "BsModule.h"
#include "BsColor.h"
#include "BsAABox.h"

namespace BansheeEngine
{
	/**
	 * @brief	Helper class that makes it easy to construct and render various common 3D shapes.
	 */
	class BS_EXPORT DrawHelper3D : public DrawHelperTemplate<Vector3>, public Module<DrawHelper3D>
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
		void aabox(const AABox& box, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @copydoc	DrawHelperTemplate::line_Pixel
		 */
		void line_Pixel(const Vector3& a, const Vector3& b, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @copydoc	DrawHelperTemplate::line_AA
		 */
		void line_AA(const Vector3& a, const Vector3& b, float width, float borderWidth, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @copydoc	DrawHelperTemplate::lineList_Pixel
		 */
		void lineList_Pixel(const Vector<Vector3>& linePoints, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @copydoc	DrawHelperTemplate::lineList_AA
		 */
		void lineList_AA(const Vector<Vector3>& linePoints, float width, float borderWidth, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @brief	Constructs a one-pixel wide line and draws it in the specified camera. 
		 *
		 * @param	camera		Camera to draw the line in.
		 * @param	x			Starting position of the line. Coordinates must be normalized in [0, 1] range. Coordinate origin is top left of the camera viewport.
		 * @param	y			End position of the line. Coordinates must be normalized in [0, 1] range. Coordinate origin is top left of the camera viewport.
		 * @param	color		Color of the line.
		 * @param	timeout		Optional timeout on how long to display the line in seconds. If 0 the line will be displayed one frame.
		 */
		void drawLine_Pixel(const HCamera& camera, const Vector3& a, const Vector3& b, const Color& color = Color::White, float timeout = 0.0f);

		/**
		 * @brief	Constructs an antialiased line with custom width and draws it in the specified camera. 
		 *
		 * @param	camera		Camera to draw the line in.
		 * @param	x			Starting position of the line. Coordinates must be normalized in [0, 1] range. Coordinate origin is top left of the camera viewport.
		 * @param	y			End position of the line. Coordinates must be normalized in [0, 1] range. Coordinate origin is top left of the camera viewport.
		 * @param	width		Width of the line in pixels.
		 * @param	borderWidth	Width of the antialiased border in pixels.
		 * @param	color		Color of the line.
		 * @param	timeout		Optional timeout on how long to display the line in seconds. If 0 the line will be displayed one frame.
		 */
		void drawLine_AA(const HCamera& camera, const Vector3& a, const Vector3& b, float width, float borderWidth, 
			const Color& color = Color::White, float timeout = 0.0f);

		/**
		 * @brief	Constructs a pixel perfect list of lines and draws them in the specified camera. 
		 *
		 * @param	camera		Camera to draw the line list in.
		 * @param	linePoints	List of line start and end points. This list must be a multiple of 2, where each start point is followed by and end point.
		 *						Coordinates must be normalized in ([0, 1], [0, 1]) range. Coordinate origin is top left of the camera viewport.
		 * @param	color		Color of the line list.
		 * @param	timeout		Optional timeout on how long to display the line list in seconds. If 0 the line list will be displayed one frame.
		 */
		void drawLineList_Pixel(const HCamera& camera, const Vector<Vector3>& linePoints, const Color& color = Color::White, float timeout = 0.0f);

		/**
		 * @brief	Constructs an list of antialiased lines with custom width and draws them in the specified camera. 
		 *
		 * @param	camera		Camera to draw the line list in.
		 * @param	linePoints	List of line start and end points. This list must be a multiple of 2, where each start point is followed by and end point.
		 *						Coordinates must be normalized in ([0, 1], [0, 1]) range. Coordinate origin is top left of the camera viewport.
		 * @param	width		Width of the line in pixels.
		 * @param	borderWidth	Width of the antialiased border in pixels.
		 * @param	color		Color of the line list.
		 * @param	timeout		Optional timeout on how long to display the line list in seconds. If 0 the line list will be displayed one frame.
		 */
		void drawLineList_AA(const HCamera& camera, const Vector<Vector3>& linePoints, float width, float borderWidth, 
			const Color& color = Color::White, float timeout = 0.0f);

		/**
		 * @brief	Constructs an axis aligned box and draws it in the specified camera.
		 *
		 * @param	camera		Camera to draw the box in.
		 * @param	box			Box to draw.
		 * @param	color		Color of the box.
		 * @param	timeout		Optional timeout on how long to display the box in seconds. If 0 the box will be displayed one frame.
		 */
		void drawAABox(const HCamera& camera, const AABox& box, const Color& color = Color::White, float timeout = 0.0f);

	protected:
		/**
		 * @copydoc	DrawHelperTemplate::line_AA(const Vector2&, const Vector2&, float, float, const Color&, UINT8*, UINT8*, UINT32, UINT32, UINT32*, UINT32)
		 */
		void line_AA(const Vector3& a, const Vector3& b, float width, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors, 
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset);

		/**
		 * @copydoc	DrawHelperTemplate::polygon_AA(const Vector<Vector2>&, float, const Color&, UINT8*, UINT8*, UINT32, UINT32, UINT32*, UINT32)
		 */
		void polygon_AA(const Vector<Vector3>& points, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors, 
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset);

		/**
		 * @brief	Fills the provided buffers with position and index data representing an axis aligned box.
		 *
		 * @param	box				Box to create geometry for.
		 * @param	outVertices		Output buffer that will store the vertex position data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and color buffer)
		 * @param	outIndices		Output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 */
		void aabox(const AABox& box, UINT8* outVertices, UINT32 vertexOffset, UINT32 vertexStride, 
			UINT32* outIndices, UINT32 indexOffset);

	private:
		/**
		 * @brief	Calculates the center of the provided vertices.
		 * 
		 * @param	vertices		Buffer containing vertices. Vertices must be of three dimensions at least.
		 * @param	numVertices		Number of vertices to calculate the center for.
		 * @param	vertexStride	Number of bytes between two vertices in the buffer.
		 *
		 * @returns	Center point of the vertices.
		 */
		Vector3 calcCenter(UINT8* vertices, UINT32 numVertices, UINT32 vertexStride);

		VertexDataDescPtr mVertexDesc;
	};
}