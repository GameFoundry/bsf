#pragma once

#include "BsPrerequisites.h"
#include "BsDrawHelperTemplate.h"
#include "BsModule.h"
#include "BsColor.h"
#include "BsAABox.h"
#include "BsRect3.h"

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
		void wireAABox(const AABox& box, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

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
		void solidAABox(const AABox& box, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

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
		void wireSphere(const Sphere& sphere, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset, UINT32 quality = 10);

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
		void solidSphere(const Sphere& sphere, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset, UINT32 quality = 1);

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
		void wireArc(const Vector3& center, float radius, const Vector3& normal, Degree startAngle, Degree amountAngle,
			const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset, UINT32 quality = 10);

		/**
		 * @brief	Fills the mesh data with vertices representing a solid arc.
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
		 * 			  Enough space for ((quality + 1) * 5 + 1) vertices 
		 *			  Enough space for (((quality + 1) * 5 - 1) * 3) indices
		 *
		 *			Primitives are output in the form of a triangle list.
		 */
		void solidArc(const Vector3& center, float radius, const Vector3& normal, Degree startAngle, Degree amountAngle,
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
		void wireDisc(const Vector3& center, float radius, const Vector3& normal, const MeshDataPtr& meshData, 
			UINT32 vertexOffset, UINT32 indexOffset, UINT32 quality = 10);

		/**
		 * @brief	Fills the mesh data with vertices representing a solid disc.
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
		 * 			  Enough space for ((quality + 1) * 5 + 1) vertices 
		 *			  Enough space for (((quality + 1) * 5 - 1) * 3) indices
		 *
		 *			Primitives are output in the form of a triangle list.
		 */
		void solidDisc(const Vector3& center, float radius, const Vector3& normal, const MeshDataPtr& meshData, 
			UINT32 vertexOffset, UINT32 indexOffset, UINT32 quality = 10);

		/**
		 * @brief	Fills the mesh data with vertices representing an outline of an camera frustum. Frustum will
		 *			be facing -z and be positioned at world origin.
		 *
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
		void wireFrustum(float aspect, Degree FOV, float near, float far,
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
		void solidCone(const Vector3& base, const Vector3& normal, float height, float radius,
			const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset, UINT32 quality = 10);

		/**
		 * @brief	Fills the mesh data with vertices representing a quad (2 triangles).
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
		 * 			  Enough space for 4 vertices and 6 indices
		 *
		 *			Primitives are output in the form of a triangle list.
		 */
		void solidQuad(const Rect3& area, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @copydoc	DrawHelperTemplate::pixelLine
		 */
		void pixelLine(const Vector3& a, const Vector3& b, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @copydoc	DrawHelperTemplate::antialiasedLine
		 */
		void antialiasedLine(const Vector3& a, const Vector3& b, const Vector3& up, float width, float borderWidth, const Color& color, 
			const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @copydoc	DrawHelperTemplate::pixelLineList
		 */
		void pixelLineList(const Vector<Vector3>& linePoints, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

		/**
		 * @copydoc	DrawHelperTemplate::antialiasedLineList
		 */
		void antialiasedLineList(const Vector<Vector3>& linePoints, const Vector3& up, float width, float borderWidth, 
			const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset);

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
		 * @brief	Constructs a one-pixel wide line and draws it in the specified camera. 
		 *
		 * @param	camera		Camera to draw the line in.
		 * @param	x			Starting position of the line. Coordinates must be normalized in [0, 1] range. Coordinate origin is top left of the camera viewport.
		 * @param	y			End position of the line. Coordinates must be normalized in [0, 1] range. Coordinate origin is top left of the camera viewport.
		 * @param	color		Color of the line.
		 * @param	timeout		Optional timeout on how long to display the line in seconds. If 0 the line will be displayed one frame.
		 */
		void drawPixelLine(const HCamera& camera, const Vector3& a, const Vector3& b, const Color& color = Color::White, float timeout = 0.0f);

		/**
		 * @brief	Constructs an antialiased line with custom width and draws it in the specified camera. 
		 *
		 * @param	camera		Camera to draw the line in.
		 * @param	x			Starting position of the line. Coordinates must be normalized in [0, 1] range. Coordinate origin is top left of the camera viewport.
		 * @param	y			End position of the line. Coordinates must be normalized in [0, 1] range. Coordinate origin is top left of the camera viewport.
		 * @param	up			Up direction to which the line will run perpendicular to.
		 * @param	width		Width of the line in pixels.
		 * @param	borderWidth	Width of the antialiased border in pixels.
		 * @param	color		Color of the line.
		 * @param	timeout		Optional timeout on how long to display the line in seconds. If 0 the line will be displayed one frame.
		 */
		void drawAntialiasedLine(const HCamera& camera, const Vector3& a, const Vector3& b, const Vector3& up, float width, float borderWidth, 
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
		void drawPixelLineList(const HCamera& camera, const Vector<Vector3>& linePoints, const Color& color = Color::White, float timeout = 0.0f);

		/**
		 * @brief	Constructs an list of antialiased lines with custom width and draws them in the specified camera. 
		 *
		 * @param	camera		Camera to draw the line list in.
		 * @param	linePoints	List of line start and end points. This list must be a multiple of 2, where each start point is followed by and end point.
		 *						Coordinates must be normalized in ([0, 1], [0, 1]) range. Coordinate origin is top left of the camera viewport.
		 * @param	up			Up direction to which the line will run perpendicular to.
		 * @param	width		Width of the line in pixels.
		 * @param	borderWidth	Width of the antialiased border in pixels.
		 * @param	color		Color of the line list.
		 * @param	timeout		Optional timeout on how long to display the line list in seconds. If 0 the line list will be displayed one frame.
		 */
		void drawAntialiasedLineList(const HCamera& camera, const Vector<Vector3>& linePoints, const Vector3& up, float width, float borderWidth, 
			const Color& color = Color::White, float timeout = 0.0f);

		/**
		 * @brief	Constructs an axis aligned box and draws it in the specified camera.
		 *
		 * @param	camera		Camera to draw the box in.
		 * @param	box			Box to draw.
		 * @param	color		Color of the box.
		 * @param	timeout		Optional timeout on how long to display the box in seconds. If 0 the box will be displayed one frame.
		 */
		void drawWireAABox(const HCamera& camera, const AABox& box, const Color& color = Color::White, float timeout = 0.0f);

	protected:
		/**
		 * @copydoc	DrawHelperTemplate::antialiasedLine(const Vector3&, const Vector3&, const Vector3&, float, float, const Color&, UINT8*, UINT8*, UINT32, UINT32, UINT32*, UINT32)
		 */
		void antialiasedLine(const Vector3& a, const Vector3& b, const Vector3& up, float width, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors, 
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset);

		/**
		 * @copydoc	DrawHelperTemplate::antialiasedPolygon(const Vector<Vector3>&, const Vector3&, float, const Color&, UINT8*, UINT8*, UINT32, UINT32, UINT32*, UINT32)
		 */
		void antialiasedPolygon(const Vector<Vector3>& points, const Vector3& up, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors, 
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset);

		/**
		 * @brief	Fills the provided buffers with position and index data representing an outline of an axis aligned box.
		 *
		 * @param	box				Box to create geometry for.
		 * @param	outVertices		Output buffer that will store the vertex position data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes.
		 * @param	outIndices		Output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 */
		void wireAABox(const AABox& box, UINT8* outVertices, UINT32 vertexOffset, UINT32 vertexStride, 
			UINT32* outIndices, UINT32 indexOffset);

		/**
		 * @brief	Fills the provided buffers with position and index data representing a solid axis aligned box.
		 *
		 * @param	box				Box to create geometry for.
		 * @param	outVertices		Output buffer that will store the vertex position data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and normal buffer)
		 * @param	outIndices		Output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 */
		void solidAABox(const AABox& box, UINT8* outVertices, UINT8* outNormals, UINT32 vertexOffset, UINT32 vertexStride, 
			UINT32* outIndices, UINT32 indexOffset);

		/**
		 * @brief	Fills the provided buffers with position and index data representing a sphere.
		 *
		 * @param	sphere			Sphere to create geometry for.
		 * @param	outVertices		Output buffer that will store the vertex position data.
		 * @param	outNormals		Output buffer that will store the vertex normal data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and normal buffer)
		 * @param	outIndices		Output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * @param	quality			Represents the level of tessellation the sphere will have. Higher level means higher quality
		 *							but also more vertices and primitives.
		 */
		void solidSphere(const Sphere& sphere, UINT8* outVertices, UINT8* outNormals, UINT32 vertexOffset, UINT32 vertexStride,
			UINT32* outIndices, UINT32 indexOffset, UINT32 quality);

		/**
		 * @brief	Fills the provided buffers with position and index data representing an outline of an arc.
		 *
		 * @param	center			Center of the arc to generate geometry for.
		 * @param	radius			Radius of the arc to generate geometry for.
		 * @param	normal			Normal around which the arc is generated. Arc geometry will be perpendicular to the normal.
		 * @param	startAngle		Angle at which the arc starts.
		 * @param	amountAngle		Angle that the arc spans.
		 * @param	outVertices		Output buffer that will store the vertex position data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes.
		 * @param	outIndices		Output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * @param	quality			Represents the level of tessellation the arc will have. Higher level means higher quality
		 *							but also more vertices and primitives.
		 */
		void wireArc(const Vector3& center, float radius, const Vector3& normal, Degree startAngle, Degree amountAngle, 
			UINT8* outVertices, UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset, UINT32 quality);

		/**
		 * @brief	Fills the provided buffers with position and index data representing a solid arc.
		 *
		 * @param	center			Center of the arc to generate geometry for.
		 * @param	radius			Radius of the arc to generate geometry for.
		 * @param	normal			Normal around which the arc is generated. Arc geometry will be perpendicular to the normal.
		 * @param	startAngle		Angle at which the arc starts.
		 * @param	amountAngle		Angle that the arc spans.
		 * @param	outVertices		Output buffer that will store the vertex position data.
		 * @param	outNormals		Output buffer that will store the vertex normal data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and normal buffer)
		 * @param	outIndices		Output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * @param	quality			Represents the level of tessellation the arc will have. Higher level means higher quality
		 *							but also more vertices and primitives.
		 */
		void solidArc(const Vector3& center, float radius, const Vector3& normal, Degree startAngle, Degree amountAngle, 
			UINT8* outVertices, UINT8* outNormals, UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset, UINT32 quality);

		/**
		 * @brief	Fills the provided buffers with position and index data representing an outline of an axis aligned box.
		 *
		 * @param	aspect			Aspect ratio (width / height).
		 * @param	FOV				Horizontal field of view angle.
		 * @param	near			Distance to near clipping plane.
		 * @param	far				Distance to far clipping plane.
		 * @param	outVertices		Output buffer that will store the vertex position data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes.
		 * @param	outIndices		Output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 */
		void wireFrustum(float aspect, Degree FOV, float near, float far,
			UINT8* outVertices, UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset);

		/**
		 * @brief	Fills the provided buffers with position and index data representing a solid cone.
		 *
		 * @param	base			World position of the cone base.
		 * @param	normal			Direction of the pointed part of the cone.
		 * @param	height			Cone height (distance from base to the top).
		 * @param	radius			Cone radius (distance from base center to outer edge).
		 * @param	outVertices		Output buffer that will store the vertex position data.
		 * @param	outNormals		Output buffer that will store the vertex normal data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and normal buffer)
		 * @param	outIndices		Output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 * @param	quality			Represents the level of tessellation the cone will have. Higher level means higher quality
		 *							but also more vertices and primitives.
		 */
		void solidCone(const Vector3& base, const Vector3& normal, float height, float radius,
			UINT8* outVertices, UINT8* outNormals, UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset, UINT32 quality);

		/**
		 * @brief	Fills the provided buffers with position and index data representing a solid quad.
		 *
		 * @param	area			Area covered by the quad.
		 * @param	outVertices		Output buffer that will store the vertex position data.
		 * @param	outNormals		Output buffer that will store the vertex normal data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and normal buffer)
		 * @param	outIndices		Output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 */
		void solidQuad(const Rect3& area, UINT8* outVertices, UINT8* outNormals, UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset);

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
		UINT32 subdivideTriangleOnSphere(const Vector3& center, float radius, UINT32 numLevels,
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
		void generateArcVertices(const Vector3& center, const Vector3& up, float radius, Degree startAngle, Degree angleAmount, UINT32 numVertices,
			UINT8* outvertices, UINT32 vertexOffset, UINT32 vertexStride);

		VertexDataDescPtr mVertexDesc;
	};
}