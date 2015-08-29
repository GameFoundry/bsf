#include "BsShapeMeshes2D.h"
#include "BsRect2.h"
#include "BsMesh.h"
#include "BsTime.h"
#include "BsVector2.h"
#include "BsMaterial.h"
#include "BsPass.h"
#include "BsCoreApplication.h"
#include "BsRenderQueue.h"
#include "BsCCamera.h"
#include "BsCoreThreadAccessor.h"
#include "BsBuiltinResources.h"
#include "BsVertexDataDesc.h"

namespace BansheeEngine
{
	const UINT32 ShapeMeshes2D::NUM_VERTICES_AA_LINE = 8;
	const UINT32 ShapeMeshes2D::NUM_INDICES_AA_LINE = 30;

	void ShapeMeshes2D::solidQuad(const Rect2& area, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
	{
		UINT32* indexData = meshData->getIndices32();
		UINT8* positionData = meshData->getElementData(VES_POSITION);

		assert((vertexOffset + 4) <= meshData->getNumVertices());
		assert((indexOffset + 6) <= meshData->getNumIndices());

		Vector<Vector2> points;
		points.push_back(Vector2(area.x, area.y));
		points.push_back(Vector2(area.x + area.width, area.y));
		points.push_back(Vector2(area.x + area.width, area.y + area.height));
		points.push_back(Vector2(area.x, area.y + area.height));	

		pixelSolidPolygon(points, positionData, vertexOffset, meshData->getVertexDesc()->getVertexStride(), indexData, indexOffset);
	}

	void ShapeMeshes2D::pixelLine(const Vector2& a, const Vector2& b, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
	{
		UINT32* indexData = meshData->getIndices32();
		UINT8* positionData = meshData->getElementData(VES_POSITION);

		assert((vertexOffset + 2) <= meshData->getNumVertices());
		assert((indexOffset + 2) <= meshData->getNumIndices());

		pixelLine(a, b, positionData, vertexOffset, meshData->getVertexDesc()->getVertexStride(), indexData, indexOffset);
	}

	void ShapeMeshes2D::antialiasedLine(const Vector2& a, const Vector2& b, float width, float borderWidth, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
	{
		UINT32* indexData = meshData->getIndices32();
		UINT8* positionData = meshData->getElementData(VES_POSITION);
		UINT8* colorData = meshData->getElementData(VES_COLOR);

		assert((vertexOffset + NUM_VERTICES_AA_LINE) <= meshData->getNumVertices());
		assert((indexOffset + NUM_INDICES_AA_LINE) <= meshData->getNumIndices());

		antialiasedLine(a, b, width, borderWidth, color, positionData, colorData, vertexOffset, meshData->getVertexDesc()->getVertexStride(), indexData, indexOffset);
	}

	void ShapeMeshes2D::pixelLineList(const Vector<Vector2>& linePoints, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
	{
		assert(linePoints.size() % 2 == 0);

		assert((vertexOffset + linePoints.size() * 2) <= meshData->getNumVertices());
		assert((indexOffset + linePoints.size() * 2) <= meshData->getNumIndices());

		UINT32 curVertOffset = vertexOffset;
		UINT32 curIdxOffset = indexOffset;

		UINT32* indexData = meshData->getIndices32();
		UINT8* positionData = meshData->getElementData(VES_POSITION);

		UINT32 numPoints = (UINT32)linePoints.size();
		for(UINT32 i = 0; i < numPoints; i += 2)
		{
			pixelLine(linePoints[i], linePoints[i + 1], positionData, curVertOffset, meshData->getVertexDesc()->getVertexStride(), indexData, curIdxOffset);

			curVertOffset += 2;
			curIdxOffset += 2;
		}
	}

	void ShapeMeshes2D::antialiasedLineList(const Vector<Vector2>& linePoints, float width, float borderWidth, const Color& color,
		const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
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
			antialiasedLine(linePoints[i], linePoints[i + 1], width, borderWidth, color, positionData, colorData, 
				curVertOffset, meshData->getVertexDesc()->getVertexStride(), indexData, curIdxOffset);

			curVertOffset += NUM_VERTICES_AA_LINE;
			curIdxOffset += NUM_INDICES_AA_LINE;
		}
	}

	void ShapeMeshes2D::pixelWirePolygon(const Vector<Vector2>& points, UINT8* outVertices, UINT8* outColors,
		UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset)
	{
		INT32 numPoints = (INT32)points.size();
		UINT32 curVertOffset = vertexOffset;
		UINT32 curIdxOffset = indexOffset;
		for (INT32 i = 0, j = numPoints - 1; i < numPoints; j = i++)
		{
			pixelLine(points[j], points[i], outVertices, curVertOffset, vertexStride, outIndices, curIdxOffset);
			curVertOffset += 2;
			curIdxOffset += 2;
		}
	}

	void ShapeMeshes2D::pixelSolidPolygon(const Vector<Vector2>& points, UINT8* outVertices,
		UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset)
	{
		outVertices += (vertexOffset * vertexStride);

		for (auto& point : points)
		{
			Vector2* vertices = (Vector2*)outVertices;
			(*vertices) = point;

			outVertices += vertexStride;
		}

		outIndices += indexOffset;
		INT32 numPoints = (INT32)points.size();
		UINT32 idxCnt = 0;
		for (int i = 2; i < numPoints; i++)
		{
			outIndices[idxCnt++] = vertexOffset;
			outIndices[idxCnt++] = vertexOffset + i - 1;
			outIndices[idxCnt++] = vertexOffset + i;
		}
	}

	void ShapeMeshes2D::pixelLine(const Vector2& a, const Vector2& b, UINT8* outVertices,
		UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset)
	{
		outVertices += (vertexOffset * vertexStride);

		Vector2* vertices = (Vector2*)outVertices;
		(*vertices) = a;

		vertices = (Vector2*)(outVertices + vertexStride);
		(*vertices) = b;

		outIndices += indexOffset;
		outIndices[0] = vertexOffset + 0;
		outIndices[1] = vertexOffset + 1;
	}

	void ShapeMeshes2D::antialiasedLine(const Vector2& a, const Vector2& b, float width, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors,
		UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset)
	{
		Vector2 dir = b - a;
		dir.normalize();

		Vector2 nrm(dir.y, -dir.x);

		Vector<Vector2> points(4);

		float r = width - 1.0f;
		r *= 0.5f;
		if (r < 0.01f) 
			r = 0.01f;

		dir = dir * r;
		nrm = nrm * r;

		Vector2 v0 = a - dir - nrm;
		Vector2 v1 = a - dir + nrm;
		Vector2 v2 = b + dir + nrm;
		Vector2 v3 = b + dir - nrm;

		points[0] = v0;
		points[1] = v1;
		points[2] = v2;
		points[3] = v3;

		antialiasedPolygon(points, borderWidth, color, outVertices, outColors, vertexOffset, vertexStride, outIndices, indexOffset);
	}

	void ShapeMeshes2D::antialiasedPolygon(const Vector<Vector2>& points, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors,
		UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset)
	{
		UINT32 numCoords = (UINT32)points.size();

		outVertices += vertexOffset * vertexStride;
		outColors += vertexOffset * vertexStride;
		Vector<Vector2> tempNormals(numCoords);

		for(UINT32 i = 0, j = numCoords - 1; i < numCoords; j = i++)
		{
			const Vector2& v0 = points[j];
			const Vector2& v1 = points[i];

			Vector2 d = v1 - v0;
			d.normalize();

			// Rotate by 90 degrees
			std::swap(d.x, d.y); // TODO - Not properly ported
			d.y = -d.y;

			tempNormals[j] = d;

			// Also start populating the vertex array
			Vector2* vertices = (Vector2*)outVertices;
			*vertices = v1;

			UINT32* colors = (UINT32*)outColors;
			*colors = color.getAsRGBA();

			outVertices += vertexStride;
			outColors += vertexStride;
		}

		Color transparentColor = color;
		transparentColor.a = 0.0f;

		for(UINT32 i = 0, j = numCoords - 1; i < numCoords; j = i++)
		{
			const Vector2& n0 = tempNormals[j];
			const Vector2& n1 = tempNormals[i];

			Vector2 avgNrm = (n0 + n1) * 0.5f;
			float magSqrd = avgNrm.squaredLength();

			if (magSqrd > 0.000001f)
			{
				float scale = 1.0f / magSqrd;
				if (scale > 10.0f) 
					scale = 10.0f;

				avgNrm = avgNrm * scale;
			}

			Vector2 tempCoord = points[i] + avgNrm * borderWidth;

			// Move it to the vertex array
			Vector2* vertices = (Vector2*)outVertices;
			*vertices = tempCoord;

			UINT32* colors = (UINT32*)outColors;
			*colors = transparentColor.getAsRGBA();

			outVertices += vertexStride;
			outColors += vertexStride;
		}

		// Populate index buffer
		outIndices += indexOffset;

		UINT32 idxCnt = 0;
		for(UINT32 i = 0, j = numCoords - 1; i < numCoords; j = i++)
		{
			outIndices[idxCnt++] = vertexOffset + i;
			outIndices[idxCnt++] = vertexOffset + j;
			outIndices[idxCnt++] = vertexOffset + numCoords + j;

			outIndices[idxCnt++] = vertexOffset + numCoords + j;
			outIndices[idxCnt++] = vertexOffset + numCoords + i;
			outIndices[idxCnt++] = vertexOffset + i;
		}

		for(UINT32 i = 2; i < numCoords; ++i)
		{
			outIndices[idxCnt++] = vertexOffset + 0;
			outIndices[idxCnt++] = vertexOffset + i - 1;
			outIndices[idxCnt++] = vertexOffset + i;
		}
	}

	Rect2 ShapeMeshes2D::normalizedCoordToClipSpace(const Rect2& area)
	{
		Rect2 clipSpaceRect;
		clipSpaceRect.x = area.x * 2.0f - 1.0f;
		clipSpaceRect.width = area.width * 2.0f;
		clipSpaceRect.y = -area.y * 2.0f + 1.0f;
		clipSpaceRect.height = area.height * -2.0f;

		return clipSpaceRect;
	}

	Vector2 ShapeMeshes2D::normalizedCoordToClipSpace(const Vector2& pos)
	{
		return Vector2(pos.x * 2.0f - 1.0f, -pos.y * 2.0f + 1.0f);
	}
}