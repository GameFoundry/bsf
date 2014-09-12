#pragma once

#include "BsPrerequisites.h"
#include "BsDebugDrawMaterialInfo.h"
#include "BsColor.h"
#include "BsAABox.h"

namespace BansheeEngine
{
	/**
	 * @brief	Types of coordinates used in 2D debug drawing.
	 */
	enum class DebugDrawCoordType
	{
		Pixel,
		Normalized
	};

	/**
	 * @brief	Internal type that determines how is a debug object drawn.
	 */
	enum class DebugDrawType
	{
		ClipSpace,
		ScreenSpace,
		WorldSpace
	};

	/**
	 * @brief	Command containing all data requires for drawing a single
	 *			debug draw object.
	 */
	struct DebugDrawCommand
	{
		HMesh mesh;

		DebugDraw2DClipSpaceMatInfo matInfo2DClipSpace;
		DebugDraw2DScreenSpaceMatInfo matInfo2DScreenSpace;
		DebugDraw3DMatInfo matInfo3D;

		DebugDrawType type;
		Vector3 worldCenter;
		float endTime;
	};

	/**
	 * @brief	Abstract interface for a draw helper class. Allows a set of draw commands to be queued
	 *			and retrieved by the renderer.
	 */
	class BS_EXPORT DrawHelperTemplateBase
	{
	public:
		static const UINT32 NUM_VERTICES_AA_LINE;
		static const UINT32 NUM_INDICES_AA_LINE;

		/**
		 * @brief	Called by the renderer when it is ready to render objects into the provided camera.
		 */
		void render(const HCamera& camera, DrawList& drawList);

	protected:
		UnorderedMap<const Viewport*, Vector<DebugDrawCommand>> mCommandsPerViewport;
	};

	/**
	 * @brief	Provides dimension-independant methods used for creating geometry 
	 *			and drawing various common objects.
	 */
	template <class T>
	class BS_EXPORT DrawHelperTemplate : public DrawHelperTemplateBase
	{
	protected:
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
		 * 			  T		  VES_POSITION
		 * 			  UINT32  VES_COLOR
		 * 			  32bit index buffer
		 * 			  Enough space for 2 vertices and 2 indices
		 *
		 *			Primitives are output in the form of a line list.
		 */
		void pixelLine(const T& a, const T& b, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
		{
			UINT32* indexData = meshData->getIndices32();
			UINT8* positionData = meshData->getElementData(VES_POSITION);
			UINT8* colorData = meshData->getElementData(VES_COLOR);

			assert((vertexOffset + 2) <= meshData->getNumVertices());
			assert((indexOffset + 2) <= meshData->getNumIndices());

			pixelLine(a, b, color, positionData, colorData, vertexOffset, meshData->getVertexDesc()->getVertexStride(), indexData, indexOffset);
		}

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
		 * 			  T		  VES_POSITION
		 * 			  UINT32  VES_COLOR
		 * 			  32bit index buffer
		 *			  Enough space for 8 vertices and 30 indices
		 *
		 *			Primitives are output in the form of a triangle list.
		 */
		void antialiasedLine(const T& a, const T& b, const T& up, float width, float borderWidth, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
		{
			UINT32* indexData = meshData->getIndices32();
			UINT8* positionData = meshData->getElementData(VES_POSITION);
			UINT8* colorData = meshData->getElementData(VES_COLOR);

			assert((vertexOffset + NUM_VERTICES_AA_LINE) <= meshData->getNumVertices());
			assert((indexOffset + NUM_INDICES_AA_LINE) <= meshData->getNumIndices());

			antialiasedLine(a, b, up, width, borderWidth, color, positionData, colorData, vertexOffset, meshData->getVertexDesc()->getVertexStride(), indexData, indexOffset);
		}

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
		 * 			  T		  VES_POSITION
		 * 			  UINT32  VES_COLOR
		 * 			  32bit index buffer
		 * 			  Enough space for (numLines * 2) vertices and (numLines * 2) indices
		 *
		 *			Primitives are output in the form of a line list.
		 */
		void pixelLineList(const typename Vector<T>& linePoints, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
		{
			assert(linePoints.size() % 2 == 0);

			assert((vertexOffset + linePoints.size() * 2) <= meshData->getNumVertices());
			assert((indexOffset + linePoints.size() * 2) <= meshData->getNumIndices());

			UINT32 curVertOffset = vertexOffset;
			UINT32 curIdxOffset = indexOffset;

			UINT32* indexData = meshData->getIndices32();
			UINT8* positionData = meshData->getElementData(VES_POSITION);
			UINT8* colorData = meshData->getElementData(VES_COLOR);

			UINT32 numPoints = (UINT32)linePoints.size();
			for(UINT32 i = 0; i < numPoints; i += 2)
			{
				pixelLine(linePoints[i], linePoints[i + 1], color, positionData, colorData, curVertOffset, meshData->getVertexDesc()->getVertexStride(), indexData, curIdxOffset);

				curVertOffset += 2;
				curIdxOffset += 2;
			}
		}

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
		 * 			  T		  VES_POSITION
		 * 			  UINT32  VES_COLOR
		 * 			  32bit index buffer
		 *			  Enough space for (numLines * 8) vertices and (numLines * 30) indices
		 *
		 *			Primitives are output in the form of a triangle list.
		 */
		void antialiasedLineList(const typename Vector<T>& linePoints, const T& up, float width, float borderWidth, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
		{
			assert(linePoints.size() % 2 == 0);

			assert((vertexOffset + linePoints.size() * 4) <= meshData->getNumVertices());
			assert((indexOffset + linePoints.size() * 15) <= meshData->getNumIndices());

			UINT32 curVertOffset = vertexOffset;
			UINT32 curIdxOffset = indexOffset;

			UINT32* indexData = meshData->getIndices32();
			UINT8* positionData = meshData->getElementData(VES_POSITION);
			UINT8* colorData = meshData->getElementData(VES_COLOR);

			UINT32 numPoints = (UINT32)linePoints.size();
			for(UINT32 i = 0; i < numPoints; i += 2)
			{
				antialiasedLine(linePoints[i], linePoints[i + 1], up, width, borderWidth, color, positionData, colorData, curVertOffset, meshData->getVertexDesc()->getVertexStride(), indexData, curIdxOffset);

				curVertOffset += NUM_VERTICES_AA_LINE;
				curIdxOffset += NUM_INDICES_AA_LINE;
			}
		}

	protected:
		/**
		 * @brief	Fills the provided buffers with vertices representing a per-pixel line.
		 *
		 * @param	a				Start point of the line.
		 * @param	b				End point of the line.
		 * @param	color			Color of the line.
		 * @param	outVertices		Output buffer that will store the vertex position data.
		 * @param	outColors		Output buffer that will store the vertex color data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and color buffer)
		 * @param	outIndices		Output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 */
		void pixelLine(const T& a, const T& b, const Color& color, UINT8* outVertices, UINT8* outColors, 
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset)
		{
			outVertices += (vertexOffset * vertexStride);
			outColors += (vertexOffset * vertexStride);

			T* vertices = (T*)outVertices;
			(*vertices) = a;

			vertices = (T*)(outVertices + vertexStride);
			(*vertices) = b;

			UINT32* colors = (UINT32*)outColors;
			(*colors) = color.getAsRGBA();

			colors = (UINT32*)(outColors + vertexStride);
			(*colors) = color.getAsRGBA();

			outIndices += indexOffset;
			outIndices[0] = vertexOffset + 0;
			outIndices[1] = vertexOffset + 1;
		}

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
		virtual void antialiasedLine(const T& a, const T& b, const T& up, float width, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors, 
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset) = 0;

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
		virtual void antialiasedPolygon(const typename Vector<T>& points, const T& up, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors,
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset) = 0;

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
		void pixelSolidPolygon(const typename Vector<T>& points, UINT8* outVertices, 
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset)
		{
			outVertices += (vertexOffset * vertexStride);

			for(auto& point : points)
			{
				T* vertices = (T*)outVertices;
				(*vertices) = point;

				outVertices += vertexStride;
			}

			outIndices += indexOffset;
			INT32 numPoints = (INT32)points.size();
			UINT32 idxCnt = 0;
			for(int i = 2; i < numPoints; i++)
			{
				outIndices[idxCnt++] = vertexOffset;
				outIndices[idxCnt++] = vertexOffset + i - 1;
				outIndices[idxCnt++] = vertexOffset + i;
			}
		}

		/**
		 * @brief	Fills the provided buffers with vertices representing a pixel-wide polygon border.
		 *
		 * @param	points			Points defining the polygon. First point is assumed to be the start and end point.
		 * @param	color			Color of the border.
		 * @param	outVertices		Output buffer that will store the vertex position data.
		 * @param	outColors		Output buffer that will store the vertex color data.
		 * @param	vertexOffset	Offset in number of vertices from the start of the buffer to start writing at.
		 * @param	vertexStride	Size of a single vertex, in bytes. (Same for both position and color buffer)
		 * @param	outIndices		Output buffer that will store the index data. Indices are 32bit.
		 * @param	indexOffset 	Offset in number of indices from the start of the buffer to start writing at.
		 */
		void pixelWirePolygon(const typename Vector<T>& points, const Color& color, UINT8* outVertices, UINT8* outColors, 
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset)
		{
			INT32 numPoints = (INT32)points.size();
			UINT32 curVertOffset = vertexOffset;
			UINT32 curIdxOffset = indexOffset;
			for(INT32 i = 0, j = numPoints - 1; i < numPoints; j = i++)
			{
				pixelLine(points[j], points[i], color, outVertices, outColors, curVertOffset, vertexStride, outIndices, curIdxOffset);
				curVertOffset += 2;
				curIdxOffset += 2;
			}
		}
	};
}