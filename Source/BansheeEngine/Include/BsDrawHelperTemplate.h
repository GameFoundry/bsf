//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsDebugDrawMaterialInfo.h"
#include "BsColor.h"
#include "BsAABox.h"

namespace BansheeEngine
{
	/**
	 * @brief	Provides dimension-independant methods used for creating geometry 
	 *			for various common shapes.
	 */
	template <class T>
	class BS_EXPORT DrawHelperTemplate
	{
	protected:
		
	protected:
		



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