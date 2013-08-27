#pragma once

#include "BsPrerequisites.h"
#include "CmColor.h"
#include "CmAABox.h"

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
		CM::HMesh mesh;
		CM::HMaterial material;
		DebugDrawType type;
		CM::Vector3 worldCenter;
		float endTime;
	};

	class BS_EXPORT DrawHelperTemplateBase
	{
	public:
		void render(const HCamera& camera, CM::RenderQueue& renderQueue);

	protected:
		CM::UnorderedMap<const CM::Viewport*, CM::Vector<DebugDrawCommand>::type>::type mCommandsPerViewport;
	};

	template <class T>
	class BS_EXPORT DrawHelperTemplate : public DrawHelperTemplateBase
	{
	protected:
		void line_Pixel(const T& a, const T& b, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset)
		{
			UINT32* indexData = meshData->getIndices32();
			UINT8* positionData = meshData->getElementData(VES_POSITION);
			UINT8* colorData = meshData->getElementData(VES_COLOR);

			assert((vertexOffset + 2) <= meshData->getNumVertices());
			assert((indexOffset + 2) <= meshData->getNumIndices());

			line_Pixel(a, b, color, positionData, colorData, vertexOffset, meshData->getVertexStride(), indexData, indexOffset);
		}

		void line_AA(const T& a, const T& b, float width, float borderWidth, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset)
		{
			UINT32* indexData = meshData->getIndices32();
			UINT8* positionData = meshData->getElementData(VES_POSITION);
			UINT8* colorData = meshData->getElementData(VES_COLOR);

			assert((vertexOffset + 8) <= meshData->getNumVertices());
			assert((indexOffset + 30) <= meshData->getNumIndices());

			line_AA(a, b, width, borderWidth, color, positionData, colorData, vertexOffset, meshData->getVertexStride(), indexData, indexOffset);
		}

		void lineList_Pixel(const typename CM::Vector<T>::type& linePoints, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset)
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
				line_Pixel(linePoints[i], linePoints[i + 1], color, positionData, colorData, curVertOffset, meshData->getVertexStride(), indexData, curIdxOffset);

				curVertOffset += 2;
				curIdxOffset += 2;
			}
		}

		void lineList_AA(const typename CM::Vector<T>::type& linePoints, float width, float borderWidth, const CM::Color& color, const CM::MeshDataPtr& meshData, CM::UINT32 vertexOffset, CM::UINT32 indexOffset)
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
				line_AA(linePoints[i], linePoints[i + 1], width, borderWidth, color, positionData, colorData, curVertOffset, meshData->getVertexStride(), indexData, curIdxOffset);

				curVertOffset += 8;
				curIdxOffset += 30;
			}
		}

		void line_Pixel(const T& a, const T& b, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
			CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset)
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

		void line_AA(const T& a, const T& b, float width, float borderWidth, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
			CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset)
		{
			T dir = b - a;
			dir.normalize();

			T nrm(dir.y, -dir.x); // TODO - Not properly ported

			Vector<T>::type points(4);

			float r = width - 1.0f;
			r *= 0.5f;
			if (r < 0.01f) 
				r = 0.01f;

			dir = dir * r;
			nrm = nrm * r;

			T v0 = a - dir - nrm;
			T v1 = a - dir + nrm;
			T v2 = b + dir + nrm;
			T v3 = b + dir - nrm;

			points[0] = v0;
			points[1] = v1;
			points[2] = v2;
			points[3] = v3;

			polygon_AA(points, borderWidth, color, outVertices, outColors, vertexOffset, vertexStride, outIndices, indexOffset);
		}

		void polygon_AA(const typename CM::Vector<T>::type& points, float borderWidth, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
			CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset)
		{
			UINT32 numCoords = (UINT32)points.size();

			outVertices += vertexOffset * vertexStride;
			Vector<T>::type tempNormals(numCoords);

			for(UINT32 i = 0, j = numCoords - 1; i < numCoords; j = i++)
			{
				const T& v0 = points[j];
				const T& v1 = points[i];

				T d = v1 - v0;
				d.normalize();

				// Rotate by 90 degrees
				std::swap(d.x, d.y); // TODO - Not properly ported
				d.y = -d.y;

				tempNormals[j] = d;

				// Also start populating the vertex array
				T* vertices = (T*)outVertices;
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
				const T& n0 = tempNormals[j];
				const T& n1 = tempNormals[i];

				T avgNrm = (n0 + n1) * 0.5f;
				float magSqrd = avgNrm.squaredLength();

				if (magSqrd > 0.000001f)
				{
					float scale = 1.0f / magSqrd;
					if (scale > 10.0f) 
						scale = 10.0f;

					avgNrm = avgNrm * scale;
				}

				T tempCoord = points[i] + avgNrm * borderWidth;

				// Move it to the vertex array
				T* vertices = (T*)outVertices;
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
				outIndices[idxCnt++] = i;
				outIndices[idxCnt++] = j;
				outIndices[idxCnt++] = numCoords + j;

				outIndices[idxCnt++] = numCoords + j;
				outIndices[idxCnt++] = numCoords + i;
				outIndices[idxCnt++] = i;
			}

			for(UINT32 i = 2; i < numCoords; ++i)
			{
				outIndices[idxCnt++] = 0;
				outIndices[idxCnt++] = i - 1;
				outIndices[idxCnt++] = i;
			}
		}

		void polygonFill_Pixel(const typename CM::Vector<T>::type& points, CM::UINT8* outVertices, 
			CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset)
		{
			outVertices += (vertexOffset * vertexStride);

			for(auto& point : points)
			{
				T* vertices = (T*)outVertices;
				(*vertices) = Vector2(point.x, point.y); // TODO - Not properly ported

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

		void polygonBorder_Pixel(const typename CM::Vector<T>::type& points, const CM::Color& color, CM::UINT8* outVertices, CM::UINT8* outColors, 
			CM::UINT32 vertexOffset, CM::UINT32 vertexStride, CM::UINT32* outIndices, CM::UINT32 indexOffset)
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