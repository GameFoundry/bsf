#pragma once

#include "BsPrerequisites.h"
#include "BsDebugDrawMaterialInfo.h"
#include "BsColor.h"
#include "BsAABox.h"

namespace BansheeEngine
{
	enum class DebugDrawCoordType
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

	enum class DebugDrawType
	{
		ClipSpace,
		ScreenSpace,
		WorldSpace
	};

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

	class BS_EXPORT DrawHelperTemplateBase
	{
	public:
		void render(const HCamera& camera, DrawList& drawList);

	protected:
		UnorderedMap<const Viewport*, Vector<DebugDrawCommand>> mCommandsPerViewport;
	};

	template <class T>
	class BS_EXPORT DrawHelperTemplate : public DrawHelperTemplateBase
	{
	protected:
		void line_Pixel(const T& a, const T& b, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
		{
			UINT32* indexData = meshData->getIndices32();
			UINT8* positionData = meshData->getElementData(VES_POSITION);
			UINT8* colorData = meshData->getElementData(VES_COLOR);

			assert((vertexOffset + 2) <= meshData->getNumVertices());
			assert((indexOffset + 2) <= meshData->getNumIndices());

			line_Pixel(a, b, color, positionData, colorData, vertexOffset, meshData->getVertexDesc()->getVertexStride(), indexData, indexOffset);
		}

		void line_AA(const T& a, const T& b, float width, float borderWidth, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
		{
			UINT32* indexData = meshData->getIndices32();
			UINT8* positionData = meshData->getElementData(VES_POSITION);
			UINT8* colorData = meshData->getElementData(VES_COLOR);

			assert((vertexOffset + 8) <= meshData->getNumVertices());
			assert((indexOffset + 30) <= meshData->getNumIndices());

			line_AA(a, b, width, borderWidth, color, positionData, colorData, vertexOffset, meshData->getVertexDesc()->getVertexStride(), indexData, indexOffset);
		}

		void lineList_Pixel(const typename Vector<T>& linePoints, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
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
				line_Pixel(linePoints[i], linePoints[i + 1], color, positionData, colorData, curVertOffset, meshData->getVertexDesc()->getVertexStride(), indexData, curIdxOffset);

				curVertOffset += 2;
				curIdxOffset += 2;
			}
		}

		void lineList_AA(const typename Vector<T>& linePoints, float width, float borderWidth, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
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
				line_AA(linePoints[i], linePoints[i + 1], width, borderWidth, color, positionData, colorData, curVertOffset, meshData->getVertexDesc()->getVertexStride(), indexData, curIdxOffset);

				curVertOffset += 8;
				curIdxOffset += 30;
			}
		}

		void line_Pixel(const T& a, const T& b, const Color& color, UINT8* outVertices, UINT8* outColors, 
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

		virtual void line_AA(const T& a, const T& b, float width, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors, 
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset) = 0;

		virtual void polygon_AA(const typename Vector<T>& points, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors, 
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset) = 0;

		void polygonFill_Pixel(const typename Vector<T>& points, UINT8* outVertices, 
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

		void polygonBorder_Pixel(const typename Vector<T>& points, const Color& color, UINT8* outVertices, UINT8* outColors, 
			UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset)
		{
			INT32 numPoints = (INT32)points.size();
			UINT32 curVertOffset = vertexOffset;
			UINT32 curIdxOffset = indexOffset;
			for(INT32 i = 0, j = numPoints - 1; i < numPoints; j = i++)
			{
				line_Pixel(points[j], points[i], color, outVertices, outColors, curVertOffset, vertexStride, outIndices, curIdxOffset);
				curVertOffset += 2;
				curIdxOffset += 2;
			}
		}
	};
}