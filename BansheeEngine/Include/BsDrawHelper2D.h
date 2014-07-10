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
	 * @brief	Helper class that makes it easy to construct and render various common 2D shapes.
	 */
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
		 * @copydoc	DrawHelperTemplate::line_Pixel
		 */
		void line_Pixel(const Vector2& a, const Vector2& b, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @copydoc	DrawHelperTemplate::line_AA
		 */
		void line_AA(const Vector2& a, const Vector2& b, float width, float borderWidth, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @copydoc	DrawHelperTemplate::lineList_Pixel
		 */
		void lineList_Pixel(const Vector<Vector2>& linePoints, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @copydoc	DrawHelperTemplate::lineList_AA
		 */
		void lineList_AA(const Vector<Vector2>& linePoints, float width, float borderWidth, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @brief	Constructs a quad and draws it in the specified camera. 
		 *
		 * @param	camera		Camera to draw the quad in.
		 * @param	area		Area of the quad. Coordinates must be normalized in ([0, 1], [0, 1]) range. Coordinate origin is top left of the camera viewport.
		 * @param	color		Color of the quad.
		 * @param	coordType	How to handle viewport resizing. If mode is Pixel the quad will always retain its size in pixels, and if mode is Normalized
		 *						the quad will stretch with the viewport so its normalized coordinates stay the same.
		 * @param	timeout		Optional timeout on how long to display the quad in seconds. If 0 the quad will be displayed one frame.
		 */
		void drawQuad(const HCamera& camera, const RectF& area, const Color& color = Color::White, DebugDrawCoordType coordType = DebugDrawCoordType::Pixel, float timeout = 0.0f);

		/**
		 * @brief	Constructs a pixel perfect line and draws it in the specified camera. 
		 *
		 * @param	camera		Camera to draw the line in.
		 * @param	x			Starting position of the line. Coordinates must be normalized in [0, 1] range. Coordinate origin is top left of the camera viewport.
		 * @param	y			End position of the line. Coordinates must be normalized in [0, 1] range. Coordinate origin is top left of the camera viewport.
		 * @param	color		Color of the line.
		 * @param	coordType	How to handle viewport resizing. If mode is Pixel the line will always retain its size in pixels, and if mode is Normalized
		 *						the line will stretch with the viewport so its normalized coordinates stay the same.
		 * @param	timeout		Optional timeout on how long to display the line in seconds. If 0 the line will be displayed one frame.
		 */
		void drawLine_Pixel(const HCamera& camera, const Vector2& a, const Vector2& b, const Color& color = Color::White, 
			DebugDrawCoordType coordType = DebugDrawCoordType::Pixel, float timeout = 0.0f);

		/**
		 * @brief	Constructs an antialiased line with custom width and draws it in the specified camera. 
		 *
		 * @param	camera		Camera to draw the line in.
		 * @param	x			Starting position of the line. Coordinates must be normalized in [0, 1] range. Coordinate origin is top left of the camera viewport.
		 * @param	y			End position of the line. Coordinates must be normalized in [0, 1] range. Coordinate origin is top left of the camera viewport.
		 * @param	width		Width of the line in pixels.
		 * @param	borderWidth	Width of the antialiased border in pixels.
		 * @param	color		Color of the line.
		 * @param	coordType	How to handle viewport resizing. If mode is Pixel the line will always retain its size in pixels, and if mode is Normalized
		 *						the line will stretch with the viewport so its normalized coordinates stay the same.
		 * @param	timeout		Optional timeout on how long to display the line in seconds. If 0 the line will be displayed one frame.
		 */
		void drawLine_AA(const HCamera& camera, const Vector2& a, const Vector2& b, float width, float borderWidth, 
			const Color& color = Color::White, DebugDrawCoordType coordType = DebugDrawCoordType::Pixel, float timeout = 0.0f);

		/**
		 * @brief	Constructs a pixel perfect list of lines and draws them in the specified camera. 
		 *
		 * @param	camera		Camera to draw the line list in.
		 * @param	linePoints	List of line start and end points. This list must be a multiple of 2, where each start point is followed by and end point.
		 *						Coordinates must be normalized in ([0, 1], [0, 1]) range. Coordinate origin is top left of the camera viewport.
		 * @param	color		Color of the line list.
		 * @param	coordType	How to handle viewport resizing. If mode is Pixel the line list will always retain its size in pixels, and if mode is Normalized
		 *						the line list will stretch with the viewport so its normalized coordinates stay the same.
		 * @param	timeout		Optional timeout on how long to display the line list in seconds. If 0 the line list will be displayed one frame.
		 */
		void drawLineList_Pixel(const HCamera& camera, const Vector<Vector2>& linePoints, const Color& color = Color::White, 
			DebugDrawCoordType coordType = DebugDrawCoordType::Pixel, float timeout = 0.0f);

		/**
		 * @brief	Constructs an list of antialiased lines with custom width and draws them in the specified camera. 
		 *
		 * @param	camera		Camera to draw the line list in.
		 * @param	linePoints	List of line start and end points. This list must be a multiple of 2, where each start point is followed by and end point.
		 *						Coordinates must be normalized in ([0, 1], [0, 1]) range. Coordinate origin is top left of the camera viewport.
		 * @param	width		Width of the line in pixels.
		 * @param	borderWidth	Width of the antialiased border in pixels.
		 * @param	color		Color of the line list.
		 * @param	coordType	How to handle viewport resizing. If mode is Pixel the line list will always retain its size in pixels, and if mode is Normalized
		 *						the line list will stretch with the viewport so its normalized coordinates stay the same.
		 * @param	timeout		Optional timeout on how long to display the line list in seconds. If 0 the line list will be displayed one frame.
		 */
		void drawLineList_AA(const HCamera& camera, const Vector<Vector2>& linePoints, float width, float borderWidth, 
			const Color& color = Color::White, DebugDrawCoordType coordType = DebugDrawCoordType::Pixel, float timeout = 0.0f);

	protected:
		/**
		 * @copydoc	DrawHelperTemplate::line_AA(const Vector2&, const Vector2&, float, float, const Color&, UINT8*, UINT8*, UINT32, UINT32, UINT32*, UINT32)
		 */
		void line_AA(const Vector2& a, const Vector2& b, float width, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors, 
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset);

		/**
		 * @copydoc	DrawHelperTemplate::polygon_AA(const Vector<Vector2>&, float, const Color&, UINT8*, UINT8*, UINT32, UINT32, UINT32*, UINT32)
		 */
		void polygon_AA(const Vector<Vector2>& points, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors, 
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset);

	private:
		/**
		 * @brief	Converts an area with normalized ([0, 1] range) coordinates and returns
		 *			area in clip space coordinates.
		 */
		RectF normalizedCoordToClipSpace(const RectF& area) const;

		/**
		 * @brief	Converts a point with normalized ([0, 1] range) coordinates and returns
		 *			a point in clip space coordinates.
		 */
		Vector2 normalizedCoordToClipSpace(const Vector2& pos) const;

	private:
		DebugDraw2DClipSpaceMatInfo mMaterial2DClipSpace;

		VertexDataDescPtr mVertexDesc;
	};
}