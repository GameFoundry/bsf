//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsColor.h"
#include "BsAABox.h"
#include "BsRect3.h"

namespace BansheeEngine
{
	/**
	 * @brief	Helper class for easily creating common 3D shapes.
	 */
	class BS_EXPORT ShapeMeshes3D
	{
	public:
		/**
		 * @brief	Fills the mesh data with vertices representing an outline of axis aligned box.
		 *
		 * @param	aabox			Axis aligned box to create the mesh data for
		 * @param	meshData		Mesh data that will be populated.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * 							
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector3 VES_POSITION
		 * 			  32bit index buffer
		 * 			  Enough space for 8 vertices and 24 indices
		 *
		 *			Primitives are output in the form of a line list.
		 */
		static void wireAABox(const AABox& box, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @brief	Fills the mesh data with vertices representing a solid axis aligned box.
		 *
		 * @param	aabox			Axis aligned box to create the mesh data for
		 * @param	meshData		Mesh data that will be populated.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * 							
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector3 VES_POSITION
		 *			  Vector3 VES_NORMAL
		 * 			  32bit index buffer
		 * 			  Enough space for 24 vertices and 36 indices
		 *
		 *			Primitives are output in the form of a triangle list.
		 */
		static void solidAABox(const AABox& box, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @brief	Fills the mesh data with vertices representing an outline of a sphere.
		 *
		 * @param	sphere			Sphere to get the mesh data for
		 * @param	meshData		Mesh data that will be populated.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * @param	quality			Represents the level of tessellation the sphere will have. Higher level means higher quality
		 *							but also more vertices and primitives.
		 *
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector3 VES_POSITION
		 * 			  32bit index buffer
		 * 			  Enough space for 3 * ((quality + 1) * 5) vertices 
		 *			  Enough space for 6 * ((quality + 1) * 5 - 1) indices
		 *
		 *			Primitives are output in the form of a line list.
		 */
		static void wireSphere(const Sphere& sphere, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset, UINT32 quality = 10);

		/**
		 * @brief	Fills the mesh data with vertices representing a sphere.
		 *
		 * @param	sphere			Sphere to get the mesh data for
		 * @param	meshData		Mesh data that will be populated.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * @param	quality			Represents the level of tessellation the sphere will have. Higher level means higher quality
		 *							but also more vertices and primitives.
		 *
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector3 VES_POSITION
		 *			  Vector3 VES_NORMAL
		 * 			  32bit index buffer
		 * 			  Enough space for 20 * (4 * (3 ^ quality)) vertices 
		 *			  Enough space for 20 * (4 * (3 ^ quality)) indices
		 *
		 *			Primitives are output in the form of a triangle list.
		 */
		static void solidSphere(const Sphere& sphere, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset, UINT32 quality = 1);

		/**
		 * @brief	Fills the mesh data with vertices representing an outline of an arc.
		 *
		 * @param	center			Center of the arc to generate geometry for.
		 * @param	radius			Radius of the arc to generate geometry for.
		 * @param	normal			Normal around which the arc is generated. Arc geometry will be perpendicular to the normal.
		 * @param	startAngle		Angle at which the arc starts.
		 * @param	amountAngle		Angle that the arc spans.
		 * @param	meshData		Mesh data that will be populated.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * @param	quality			Represents the level of tessellation the arc will have. Higher level means higher quality
		 *							but also more vertices and primitives.
		 *
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector3 VES_POSITION
		 * 			  32bit index buffer
		 * 			  Enough space for ((quality + 1) * 5) vertices 
		 *			  Enough space for (((quality + 1) * 5 - 1) * 2) indices
		 *
		 *			Primitives are output in the form of a line list.
		 */
		static void wireArc(const Vector3& center, float radius, const Vector3& normal, Degree startAngle, Degree amountAngle,
			const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset, UINT32 quality = 10);

		/**
		 * @brief	Fills the mesh data with vertices representing a solid double-sided arc.
		 *
		 * @param	center			Center of the arc to generate geometry for.
		 * @param	radius			Radius of the arc to generate geometry for.
		 * @param	normal			Normal around which the arc is generated. Arc geometry will be perpendicular to the normal.
		 * @param	startAngle		Angle at which the arc starts.
		 * @param	amountAngle		Angle that the arc spans.
		 * @param	meshData		Mesh data that will be populated.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * @param	quality			Represents the level of tessellation the arc will have. Higher level means higher quality
		 *							but also more vertices and primitives.
		 *
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector3 VES_POSITION
		 *			  Vector3 VES_NORMAL
		 * 			  32bit index buffer
		 * 			  Enough space for ((quality + 1) * 5 + 1) * 2 vertices 
		 *			  Enough space for (((quality + 1) * 5 - 1) * 6) indices
		 *
		 *			Primitives are output in the form of a triangle list.
		 */
		static void solidArc(const Vector3& center, float radius, const Vector3& normal, Degree startAngle, Degree amountAngle,
			const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset, UINT32 quality = 10);

		/**
		 * @brief	Fills the mesh data with vertices representing an outline of a disc.
		 *
		 * @param	center			Center of the disc to generate geometry for.
		 * @param	radius			Radius of the disc to generate geometry for.
		 * @param	normal			Normal around which the disc is generated. Disc geometry will be perpendicular to the normal.
		 * @param	meshData		Mesh data that will be populated.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * @param	quality			Represents the level of tessellation the disc will have. Higher level means higher quality
		 *							but also more vertices and primitives.
		 *
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector3 VES_POSITION
		 * 			  32bit index buffer
		 * 			  Enough space for ((quality + 1) * 5) vertices 
		 *			  Enough space for (((quality + 1) * 5 - 1) * 2) indices
		 *
		 *			Primitives are output in the form of a line list.
		 */
		static void wireDisc(const Vector3& center, float radius, const Vector3& normal, const MeshDataPtr& meshData,
			UINT32 vertexOffset, UINT32 indexOffset, UINT32 quality = 10);

		/**
		 * @brief	Fills the mesh data with vertices representing a solid double-sided disc.
		 *
		 * @param	center			Center of the disc to generate geometry for.
		 * @param	radius			Radius of the disc to generate geometry for.
		 * @param	normal			Normal around which the disc is generated. Disc geometry will be perpendicular to the normal.
		 * @param	meshData		Mesh data that will be populated.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * @param	quality			Represents the level of tessellation the disc will have. Higher level means higher quality
		 *							but also more vertices and primitives.
		 *
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector3 VES_POSITION
		 *			  Vector3 VES_NORMAL
		 * 			  32bit index buffer
		 * 			  Enough space for ((quality + 1) * 5 + 1) * 2 vertices 
		 *			  Enough space for (((quality + 1) * 5 - 1) * 6) indices
		 *
		 *			Primitives are output in the form of a triangle list.
		 */
		static void solidDisc(const Vector3& center, float radius, const Vector3& normal, const MeshDataPtr& meshData,
			UINT32 vertexOffset, UINT32 indexOffset, UINT32 quality = 10);

		/**
		 * @brief	Fills the mesh data with vertices representing an outline of an camera frustum. Frustum will
		 *			be facing -z and be positioned at world origin.
		 *
		 * @param	position		Starting point for the frustum.
		 * @param	aspect			Aspect ratio (width / height).
		 * @param	FOV				Horizontal field of view angle.
		 * @param	near			Distance to near clipping plane.
		 * @param	far				Distance to far clipping plane.
		 * @param	meshData		Mesh data that will be populated.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * 							
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector3 VES_POSITION
		 * 			  32bit index buffer
		 * 			  Enough space for 8 vertices and 24 indices
		 *
		 *			Primitives are output in the form of a line list.
		 */
		static void wireFrustum(const Vector3& position, float aspect, Degree FOV, float near, float far,
			const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @brief	Fills the mesh data with vertices representing a solid cone.
		 *
		 * @param	base			World position of the cone base.
		 * @param	normal			Direction of the pointed part of the cone.
		 * @param	height			Cone height (distance from base to the top).
		 * @param	radius			Cone radius (distance from base center to outer edge).
		 * @param	meshData		Mesh data that will be populated.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * @param	quality			Represents the level of tessellation the cone will have. Higher level means higher quality
		 *							but also more vertices and primitives.
		 *
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector3 VES_POSITION
		 *			  Vector3 VES_NORMAL
		 * 			  32bit index buffer
		 * 			  Enough space for ((quality + 1) * 4 + 1) * 2 vertices 
		 *			  Enough space for (((quality + 1) * 4 - 1) * 6) indices
		 *
		 *			Primitives are output in the form of a triangle list.
		 */
		static void solidCone(const Vector3& base, const Vector3& normal, float height, float radius,
			const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset, UINT32 quality = 10);

		/**
		 * @brief	Fills the mesh data with vertices representing a quad (4 triangles, two sided).
		 *
		 * @param	area			Area in which to draw the quad.
		 * @param	meshData		Mesh data that will be populated.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * 							
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector3 VES_POSITION
		 *			  Vector3 VES_NORMAL
		 * 			  32bit index buffer
		 * 			  Enough space for 8 vertices and 12 indices
		 *
		 *			Primitives are output in the form of a triangle list.
		 */
		static void solidQuad(const Rect3& area, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @brief	Fills the mesh data with vertices representing a per-pixel line.
		 *
		 * @param	a				Start point of the line.
		 * @param	b				End point of the line.
		 * @param	meshData		Mesh data that will be populated.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * 							
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector3	  VES_POSITION
		 * 			  32bit index buffer
		 * 			  Enough space for 2 vertices and 2 indices
		 *
		 *			Primitives are output in the form of a line list.
		 */
		static void pixelLine(const Vector3& a, const Vector3& b, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @brief	Fills the mesh data with vertices representing an anti-aliased line of specific width. Antialiasing is done using alpha blending.
		 *
		 * @param	a				Start point of the line.
		 * @param	b				End point of the line.
		 * @param	up				Up direction to which the line will run perpendicular to.
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
		 *
		 *			Primitives are output in the form of a triangle list.
		 */
		static void antialiasedLine(const Vector3& a, const Vector3& b, const Vector3& up, float width, float borderWidth, const Color& color,
			const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @brief	Fills the mesh data with vertices representing per-pixel lines.
		 *
		 * @param	linePoints		A list of start and end points for the lines. Must be a multiple of 2.
		 * @param	meshData		Mesh data that will be populated.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * 							
		 * @note	Provided MeshData must have some specific elements at least:
		 * 			  Vector3  VES_POSITION
		 * 			  32bit index buffer
		 * 			  Enough space for (numLines * 2) vertices and (numLines * 2) indices
		 *
		 *			Primitives are output in the form of a line list.
		 */
		static void pixelLineList(const Vector<Vector3>& linePoints, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @brief	Fills the mesh data with vertices representing anti-aliased lines of specific width. Antialiasing is done using alpha blending.
		 *
		 * @param	linePoints		A list of start and end points for the lines. Must be a multiple of 2.
		 * @param	up				Up direction to which the line will run perpendicular to.
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
		 *
		 *			Primitives are output in the form of a triangle list.
		 */
		static void antialiasedLineList(const Vector<Vector3>& linePoints, const Vector3& up, float width, float borderWidth,
			const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

/**
		 * @brief	Fills the provided buffers with position and index data representing an outline of an axis aligned box.
		 * 			Use ::getNumElementsWireAABox todetermine the required sizes of the output buffers.
		 *
		 * @param	box				Box to create geometry for.
		 * @param	outVertices		Pre-allocated output buffer that will store the vertex position data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes.
		 * @param	outIndices		Pre-allocated output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 */
		static void wireAABox(const AABox& box, UINT8* outVertices, UINT32 vertexOffset, UINT32 vertexStride,
			UINT32* outIndices, UINT32 indexOffset);

		/**
		 * @brief	Fills the provided buffers with position and index data representing a solid axis aligned box.
		 * 			Use ::getNumElementsAABox todetermine the required sizes of the output buffers.
		 *
		 * @param	box				Box to create geometry for.
		 * @param	outVertices		Pre-allocated output buffer that will store the vertex position data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and normal buffer)
		 * @param	outIndices		Pre-allocated output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 */
		static void solidAABox(const AABox& box, UINT8* outVertices, UINT8* outNormals, UINT32 vertexOffset, UINT32 vertexStride,
			UINT32* outIndices, UINT32 indexOffset);

		/**
		 * @brief	Fills the provided buffers with position and index data representing a sphere.
		 * 			Use ::getNumElementsSphere todetermine the required sizes of the output buffers.
		 *
		 * @param	sphere			Sphere to create geometry for.
		 * @param	outVertices		Pre-allocated output buffer that will store the vertex position data.
		 * @param	outNormals		Pre-allocated output buffer that will store the vertex normal data. Can be null if normals aren't needed.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and normal buffer)
		 * @param	outIndices		Pre-allocated output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * @param	quality			Represents the level of tessellation the sphere will have. Higher level means higher quality
		 *							but also more vertices and primitives.
		 */
		static void solidSphere(const Sphere& sphere, UINT8* outVertices, UINT8* outNormals, UINT32 vertexOffset, UINT32 vertexStride,
			UINT32* outIndices, UINT32 indexOffset, UINT32 quality);

		/**
		 * @brief	Fills the provided buffers with position and index data representing an outline of an arc. 
		 * 			Use ::getNumElementWiresArc to determine the required sizes of the output buffers.
		 *
		 * @param	center			Center of the arc to generate geometry for.
		 * @param	radius			Radius of the arc to generate geometry for.
		 * @param	normal			Normal around which the arc is generated. Arc geometry will be perpendicular to the normal.
		 * @param	startAngle		Angle at which the arc starts.
		 * @param	amountAngle		Angle that the arc spans.
		 * @param	outVertices		Pre-allocated output buffer that will store the vertex position data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes.
		 * @param	outIndices		Pre-allocated output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * @param	quality			Represents the level of tessellation the arc will have. Higher level means higher quality
		 *							but also more vertices and primitives.
		 */
		static void wireArc(const Vector3& center, float radius, const Vector3& normal, Degree startAngle, Degree amountAngle,
			UINT8* outVertices, UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset, UINT32 quality);

		/**
		 * @brief	Fills the provided buffers with position and index data representing a solid arc. 
		 * 			Use ::getNumElementsArc to determine the required sizes of the output buffers.
		 *
		 * @param	center			Center of the arc to generate geometry for.
		 * @param	radius			Radius of the arc to generate geometry for.
		 * @param	normal			Normal around which the arc is generated. Arc geometry will be perpendicular to the normal.
		 * @param	startAngle		Angle at which the arc starts.
		 * @param	amountAngle		Angle that the arc spans.
		 * @param	outVertices		Pre-allocated output buffer that will store the vertex position data.
		 * @param	outNormals		Pre-allocated output buffer that will store the vertex normal data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and normal buffer)
		 * @param	outIndices		Pre-allocated output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * @param	quality			Represents the level of tessellation the arc will have. Higher level means higher quality
		 *							but also more vertices and primitives.
		 */
		static void solidArc(const Vector3& center, float radius, const Vector3& normal, Degree startAngle, Degree amountAngle,
			UINT8* outVertices, UINT8* outNormals, UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset, UINT32 quality);

		/**
		 * @brief	Fills the provided buffers with position and index data representing an outline of a camera frustum.
		 * 			Use ::getNumElementsFrustum to determine the required sizes of the output buffers.
		 *
		 * @param	position		Starting point for the frustum.
		 * @param	aspect			Aspect ratio (width / height).
		 * @param	FOV				Horizontal field of view angle.
		 * @param	near			Distance to near clipping plane.
		 * @param	far				Distance to far clipping plane.
		 * @param	outVertices		Pre-allocated output buffer that will store the vertex position data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes.
		 * @param	outIndices		Pre-allocated output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 */
		static void wireFrustum(const Vector3& position, float aspect, Degree FOV, float near, float far,
			UINT8* outVertices, UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset);

		/**
		 * @brief	Fills the provided buffers with position and index data representing a solid cone.
		 * 			Use ::getNumElementsCone to determine the required sizes of the output buffers.
		 *
		 * @param	base			World position of the cone base.
		 * @param	normal			Direction of the pointed part of the cone.
		 * @param	height			Cone height (distance from base to the top).
		 * @param	radius			Cone radius (distance from base center to outer edge).
		 * @param	outVertices		Pre-allocated output buffer that will store the vertex position data.
		 * @param	outNormals		Pre-allocated output buffer that will store the vertex normal data. Can be null if normals aren't needed.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and normal buffer)
		 * @param	outIndices		Pre-allocated output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * @param	quality			Represents the level of tessellation the cone will have. Higher level means higher quality
		 *							but also more vertices and primitives.
		 */
		static void solidCone(const Vector3& base, const Vector3& normal, float height, float radius,
			UINT8* outVertices, UINT8* outNormals, UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset, UINT32 quality);

		/**
		 * @brief	Fills the provided buffers with position and index data representing a solid quad.
		 * 			Use ::getNumElementsCone to determine the required sizes of the output buffers.
		 *
		 * @param	area			Area covered by the quad.
		 * @param	outVertices		Pre-allocated output buffer that will store the vertex position data.
		 * @param	outNormals		Pre-allocated output buffer that will store the vertex normal data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and normal buffer)
		 * @param	outIndices		Pre-allocated output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 */
		static void solidQuad(const Rect3& area, UINT8* outVertices, UINT8* outNormals, UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset);

		/**
		 * @brief	Calculates number of vertices and indices required for geometry of a solid axis aligned box.
		 */
		static void getNumElementsAABox(UINT32& numVertices, UINT32& numIndices);

		/**
		 * @brief	Calculates number of vertices and indices required for geometry of a wireframe axis aligned box.
		 */
		static void getNumElementsWireAABox(UINT32& numVertices, UINT32& numIndices);

		/**
		 * @brief	Calculates number of vertices and indices required for geometry of a solid sphere
		 *			of the specified quality.
		 */
		static void getNumElementsSphere(UINT32 quality, UINT32& numVertices, UINT32& numIndices);

		/**
		 * @brief	Calculates number of vertices and indices required for geometry of a wire sphere
		 *			of the specified quality.
		 */
		static void getNumElementsWireSphere(UINT32 quality, UINT32& numVertices, UINT32& numIndices);

		/**
		 * @brief	Calculates number of vertices and indices required for geometry of a solid arc
		 *			of the specified quality.
		 */
		static void getNumElementsArc(UINT32 quality, UINT32& numVertices, UINT32& numIndices);

		/**
		 * @brief	Calculates number of vertices and indices required for geometry of a wire arc
		 *			of the specified quality.
		 */
		static void getNumElementsWireArc(UINT32 quality, UINT32& numVertices, UINT32& numIndices);

		/**
		 * @brief	Calculates number of vertices and indices required for geometry of a solid desc
		 *			of the specified quality.
		 */
		static void getNumElementsDisc(UINT32 quality, UINT32& numVertices, UINT32& numIndices);

		/**
		 * @brief	Calculates number of vertices and indices required for geometry of a wire disc
		 *			of the specified quality.
		 */
		static void getNumElementsWireDisc(UINT32 quality, UINT32& numVertices, UINT32& numIndices);

		/**
		 * @brief	Calculates number of vertices and indices required for geometry of a cone
		 *			of the specified quality.
		 */
		static void getNumElementsCone(UINT32 quality, UINT32& numVertices, UINT32& numIndices);

		/**
		 * @brief	Calculates number of vertices and indices required for geometry of a frustum.
		 */
		static void getNumElementsFrustum(UINT32& numVertices, UINT32& numIndices);

		/**
		 * @brief	Calculates number of vertices and indices required for geometry of a quadrirateral.
		 */
		static void getNumElementsQuad(UINT32& numVertices, UINT32& numIndices);

		static const UINT32 NUM_VERTICES_AA_LINE;
		static const UINT32 NUM_INDICES_AA_LINE;

	protected:
		/**
		 * @brief	Fills the provided buffers with vertices representing a per-pixel line.
		 *
		 * @param	a				Start point of the line.
		 * @param	b				End point of the line.
		 * @param	outVertices		Output buffer that will store the vertex position data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and color buffer)
		 * @param	outIndices		Output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 */
		static void pixelLine(const Vector3& a, const Vector3& b, UINT8* outVertices,
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset);

		/**
		 * @brief	Fills the provided buffers with vertices representing an antialiased line with a custom width.
		 *
		 * @param	a				Start point of the line.
		 * @param	b				End point of the line.
		 * @param	up				Up direction to which the line will run perpendicular to.
		 * @param	width			Width of the line.
		 * @param	borderWidth		Width of the anti-aliased border.
		 * @param	color			Color of the line.
		 * @param	outVertices		Output buffer that will store the vertex position data.
		 * @param	outColors		Output buffer that will store the vertex color data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and color buffer)
		 * @param	outIndices		Output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 */
		static void antialiasedLine(const Vector3& a, const Vector3& b, const Vector3& up, float width, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors,
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset);

		/**
		 * @brief	Fills the provided buffers with position data and indices representing an inner 
		 *			area of a polygon (basically a normal non-antialiased polygon).
		 *
		 * @param	points			Points defining the polygon. First point is assumed to be the start and end point.
		 * @param	outVertices		Output buffer that will store the vertex position data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and color buffer)
		 * @param	outIndices		Output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 */
		static void pixelSolidPolygon(const Vector<Vector3>& points, UINT8* outVertices,
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset);

		/**
		 * @brief	Fills the provided buffers with vertices representing a pixel-wide polygon border.
		 *
		 * @param	points			Points defining the polygon. First point is assumed to be the start and end point.
		 * @param	outVertices		Output buffer that will store the vertex position data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and color buffer)
		 * @param	outIndices		Output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 */
		static void pixelWirePolygon(const Vector<Vector3>& points, UINT8* outVertices,
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset);

		/**
		 * @brief	Fills the provided buffers with vertices representing an antialiased polygon.
		 *
		 * @param	points			Points defining the polygon. First point is assumed to be the start and end point.
		 * @param	up				Up direction to which the polygon will run perpendicular to.
		 * @param	borderWidth		Width of the anti-aliased border.
		 * @param	color			Color of the polygon.
		 * @param	outVertices		Output buffer that will store the vertex position data.
		 * @param	outColors		Output buffer that will store the vertex color data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and color buffer)
		 * @param	outIndices		Output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 */
		static void antialiasedPolygon(const Vector<Vector3>& points, const Vector3& up, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors,
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset);

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
		static Vector3 calcCenter(UINT8* vertices, UINT32 numVertices, UINT32 vertexStride);

		/**
		 * @brief	Subdivides the provided triangle so it approximates a curved surface of a sphere.
		 *
		 * @param	center			Center of the sphere to approximate.
		 * @param	radius			Radius of the sphere to approximate.
		 * @param	numLevels		Number of times to subdivide the triangle. Higher number means better approximation.
		 * @param	a				First corner of the triangle. Must be normalized.
		 * @param	b				Second corner of the triangle. Must be normalized.
		 * @param	c				Third corner of the triangle. Must be normalized.
		 * @param	outVertices		Output buffer that will store the vertex position data.
		 * @param	outNormals		Output buffer that will store the vertex normal data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and color buffer)
		 *
		 * @returns	Number of vertices generated (3 * (4 ^ numLevels)).
		 */
		static UINT32 subdivideTriangleOnSphere(const Vector3& center, float radius, UINT32 numLevels,
			const Vector3& a, const Vector3& b, const Vector3& c,
			UINT8* outVertices, UINT8* outNormals, UINT32 vertexOffset, UINT32 vertexStride);

		/**
		 * @brief	Generates vertices that are part of an arc approximation.
		 *
		 * @param	center		Determines world position of the arc.
		 * @param	up			Determines rotation of the arc. Arc vertices will be perpendicular to this direction.
		 * @param	radius		Distance of arc vertices from the center.
		 * @param	startAngle	Angle in degrees to start the arc at.
		 * @param	angleAmount	Angle in degrees to extend the arc from the start angle.
		 * @param	numVertices	Number of vertices to generate for the arc. Higher number means better arc approximation. Must be 2 or higher.
		 * @param	outVertices		Output buffer that will store the vertex position data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and color buffer)
		 */
		static void generateArcVertices(const Vector3& center, const Vector3& up, float radius, Degree startAngle, Degree angleAmount, UINT32 numVertices,
			UINT8* outvertices, UINT32 vertexOffset, UINT32 vertexStride);
	};
}