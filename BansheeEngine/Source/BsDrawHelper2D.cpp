#include "BsDrawHelper2D.h"
#include "CmRectF.h"
#include "CmMesh.h"
#include "CmTime.h"
#include "CmVector2.h"
#include "CmMaterial.h"
#include "CmPass.h"
#include "CmApplication.h"
#include "CmRenderQueue.h"
#include "BsCamera.h"
#include "CmCoreThreadAccessor.h"
#include "BsBuiltinMaterialManager.h"
#include "CmVertexDataDesc.h"

namespace BansheeEngine
{
	DrawHelper2D::DrawHelper2D()
	{
		mMaterial2DClipSpace = BuiltinMaterialManager::instance().createDebugDraw2DClipSpaceMaterial();

		mVertexDesc = cm_shared_ptr<VertexDataDesc>();
		mVertexDesc->addVertElem(VET_FLOAT2, VES_POSITION);
		mVertexDesc->addVertElem(VET_COLOR, VES_COLOR);
	}

	void DrawHelper2D::quad(const RectF& area, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
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

		polygonFill_Pixel(points, positionData, vertexOffset, meshData->getVertexDesc()->getVertexStride(), indexData, indexOffset);
	}

	void DrawHelper2D::line_Pixel(const Vector2& a, const Vector2& b, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
	{
		DrawHelperTemplate<Vector2>::line_Pixel(a, b, color, meshData, vertexOffset, indexOffset);
	}

	void DrawHelper2D::line_AA(const Vector2& a, const Vector2& b, float width, float borderWidth, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
	{
		DrawHelperTemplate<Vector2>::line_AA(a, b, width, borderWidth, color, meshData, vertexOffset, indexOffset);
	}

	void DrawHelper2D::lineList_Pixel(const Vector<Vector2>& linePoints, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
	{
		DrawHelperTemplate<Vector2>::lineList_Pixel(linePoints, color, meshData, vertexOffset, indexOffset);
	}

	void DrawHelper2D::lineList_AA(const Vector<Vector2>& linePoints, float width, float borderWidth, const Color& color, const MeshDataPtr& meshData, UINT32 vertexOffset, UINT32 indexOffset)
	{
		DrawHelperTemplate<Vector2>::lineList_AA(linePoints, width, borderWidth, color, meshData, vertexOffset, indexOffset);
	}

	/************************************************************************/
	/* 								2D - DRAW	                     		*/
	/************************************************************************/

	void DrawHelper2D::drawQuad(const HCamera& camera, const RectF& area, const Color& color, DebugDrawCoordType coordType, float timeout)
	{
		const Viewport* viewport = camera->getViewport().get();

		Vector<DebugDrawCommand>& commands = mCommandsPerViewport[viewport];

		commands.push_back(DebugDrawCommand());
		DebugDrawCommand& dbgCmd = commands.back();
		dbgCmd.endTime = gTime().getTime() + timeout;

		MeshDataPtr meshData = cm_shared_ptr<MeshData, ScratchAlloc>(4, 6, mVertexDesc);

		RectF actualArea = area;
		if(coordType == DebugDrawCoordType::Normalized)
			actualArea = normalizedCoordToClipSpace(area);

		quad(actualArea, meshData, 0, 0);

		UINT32 vertexStride = mVertexDesc->getVertexStride();
		UINT8* colorData = meshData->getElementData(VES_COLOR);

		UINT32* colors = (UINT32*)colorData;
		(*colors) = color.getAsRGBA();

		colors = (UINT32*)(colorData + vertexStride);
		(*colors) = color.getAsRGBA();

		colors = (UINT32*)(colorData + vertexStride * 2);
		(*colors) = color.getAsRGBA();

		colors = (UINT32*)(colorData + vertexStride * 3);
		(*colors) = color.getAsRGBA();

		HMesh mesh = Mesh::create(meshData);

		dbgCmd.mesh = mesh;
		dbgCmd.worldCenter = Vector3::ZERO;

		if(coordType == DebugDrawCoordType::Normalized)
		{
			dbgCmd.type = DebugDrawType::ClipSpace;
			dbgCmd.matInfo2DClipSpace = mMaterial2DClipSpace;
		}
		else
		{
			dbgCmd.type = DebugDrawType::ScreenSpace;
			dbgCmd.matInfo2DScreenSpace = BuiltinMaterialManager::instance().createDebugDraw2DScreenSpaceMaterial();
		}
	}

	void DrawHelper2D::drawLine_Pixel(const HCamera& camera, const Vector2& a, const Vector2& b, const Color& color, DebugDrawCoordType coordType, float timeout)
	{
		const Viewport* viewport = camera->getViewport().get();

		Vector<DebugDrawCommand>& commands = mCommandsPerViewport[viewport];

		commands.push_back(DebugDrawCommand());
		DebugDrawCommand& dbgCmd = commands.back();
		dbgCmd.endTime = gTime().getTime() + timeout;

		MeshDataPtr meshData = cm_shared_ptr<MeshData, ScratchAlloc>(2, 2, mVertexDesc);

		Vector2 actualA = a;
		Vector2 actualB = b;
		if(coordType == DebugDrawCoordType::Normalized)
		{
			actualA = normalizedCoordToClipSpace(a);
			actualB = normalizedCoordToClipSpace(b);
		}

		line_Pixel(actualA, actualB, color, meshData, 0, 0);

		HMesh mesh = Mesh::create(meshData, MeshBufferType::Static, DOT_LINE_LIST);

		dbgCmd.mesh = mesh;
		dbgCmd.worldCenter = Vector3::ZERO;

		if(coordType == DebugDrawCoordType::Normalized)
		{
			dbgCmd.type = DebugDrawType::ClipSpace;
			dbgCmd.matInfo2DClipSpace = mMaterial2DClipSpace;
		}
		else
		{
			dbgCmd.type = DebugDrawType::ScreenSpace;
			dbgCmd.matInfo2DScreenSpace = BuiltinMaterialManager::instance().createDebugDraw2DScreenSpaceMaterial();
		}
	}

	void DrawHelper2D::drawLine_AA(const HCamera& camera, const Vector2& a, const Vector2& b, float width, float borderWidth, const Color& color, DebugDrawCoordType coordType, float timeout)
	{
		const Viewport* viewport = camera->getViewport().get();

		Vector<DebugDrawCommand>& commands = mCommandsPerViewport[viewport];

		commands.push_back(DebugDrawCommand());
		DebugDrawCommand& dbgCmd = commands.back();
		dbgCmd.endTime = gTime().getTime() + timeout;

		MeshDataPtr meshData = cm_shared_ptr<MeshData, ScratchAlloc>(8, 30, mVertexDesc);

		Vector2 actualA = a;
		Vector2 actualB = b;
		if(coordType == DebugDrawCoordType::Normalized)
		{
			actualA = normalizedCoordToClipSpace(a);
			actualB = normalizedCoordToClipSpace(b);
		}

		line_AA(actualA, actualB, width, borderWidth, color, meshData, 0, 0);

		HMesh mesh = Mesh::create(meshData);

		dbgCmd.mesh = mesh;
		dbgCmd.worldCenter = Vector3::ZERO;

		if(coordType == DebugDrawCoordType::Normalized)
		{
			dbgCmd.type = DebugDrawType::ClipSpace;
			dbgCmd.matInfo2DClipSpace = mMaterial2DClipSpace;
		}
		else
		{
			dbgCmd.type = DebugDrawType::ScreenSpace;
			dbgCmd.matInfo2DScreenSpace = BuiltinMaterialManager::instance().createDebugDraw2DScreenSpaceMaterial();
		}
	}

	void DrawHelper2D::drawLineList_Pixel(const HCamera& camera, const Vector<Vector2>& linePoints, const Color& color, 
		DebugDrawCoordType coordType, float timeout)
	{
		const Viewport* viewport = camera->getViewport().get();

		Vector<DebugDrawCommand>& commands = mCommandsPerViewport[viewport];

		commands.push_back(DebugDrawCommand());
		DebugDrawCommand& dbgCmd = commands.back();
		dbgCmd.endTime = gTime().getTime() + timeout;

		MeshDataPtr meshData = cm_shared_ptr<MeshData, ScratchAlloc>(
			(UINT32)(linePoints.size() * 2), (UINT32)(linePoints.size() * 2), mVertexDesc);

		if(coordType == DebugDrawCoordType::Normalized)
		{
			Vector<Vector2> points;
			UINT32 numPoints = (UINT32)linePoints.size();
			for(UINT32 i = 0; i < numPoints; i++)
				points.push_back(normalizedCoordToClipSpace(linePoints[i]));

			lineList_Pixel(points, color, meshData, 0, 0);
		}
		else
		{
			lineList_Pixel(linePoints, color, meshData, 0, 0);
		}		

		HMesh mesh = Mesh::create(meshData, MeshBufferType::Static, DOT_LINE_LIST);

		dbgCmd.mesh = mesh;
		dbgCmd.worldCenter = Vector3::ZERO;

		if(coordType == DebugDrawCoordType::Normalized)
		{
			dbgCmd.type = DebugDrawType::ClipSpace;
			dbgCmd.matInfo2DClipSpace = mMaterial2DClipSpace;
		}
		else
		{
			dbgCmd.type = DebugDrawType::ScreenSpace;
			dbgCmd.matInfo2DScreenSpace = BuiltinMaterialManager::instance().createDebugDraw2DScreenSpaceMaterial();
		}
	}

	void DrawHelper2D::drawLineList_AA(const HCamera& camera, const Vector<Vector2>& linePoints, float width, float borderWidth, 
		const Color& color, DebugDrawCoordType coordType, float timeout)
	{
		const Viewport* viewport = camera->getViewport().get();

		Vector<DebugDrawCommand>& commands = mCommandsPerViewport[viewport];

		commands.push_back(DebugDrawCommand());
		DebugDrawCommand& dbgCmd = commands.back();
		dbgCmd.endTime = gTime().getTime() + timeout;

		MeshDataPtr meshData = cm_shared_ptr<MeshData, ScratchAlloc>((UINT32)(linePoints.size() * 4), (UINT32)(linePoints.size() * 15), mVertexDesc);

		if(coordType == DebugDrawCoordType::Normalized)
		{
			Vector<Vector2> points;
			UINT32 numPoints = (UINT32)linePoints.size();
			for(UINT32 i = 0; i < numPoints; i++)
				points.push_back(normalizedCoordToClipSpace(linePoints[i]));

			lineList_AA(points, width, borderWidth, color, meshData, 0, 0);
		}
		else
		{
			lineList_AA(linePoints, width, borderWidth, color, meshData, 0, 0);
		}		

		HMesh mesh = Mesh::create(meshData);

		dbgCmd.mesh = mesh;
		dbgCmd.worldCenter = Vector3::ZERO;

		if(coordType == DebugDrawCoordType::Normalized)
		{
			dbgCmd.type = DebugDrawType::ClipSpace;
			dbgCmd.matInfo2DClipSpace = mMaterial2DClipSpace;
		}
		else
		{
			dbgCmd.type = DebugDrawType::ScreenSpace;
			dbgCmd.matInfo2DScreenSpace = BuiltinMaterialManager::instance().createDebugDraw2DScreenSpaceMaterial();
		}
	}

	void DrawHelper2D::line_AA(const Vector2& a, const Vector2& b, float width, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors, 
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

		polygon_AA(points, borderWidth, color, outVertices, outColors, vertexOffset, vertexStride, outIndices, indexOffset);
	}

	void DrawHelper2D::polygon_AA(const Vector<Vector2>& points, float borderWidth, const Color& color, UINT8* outVertices, UINT8* outColors, 
		UINT32 vertexOffset, UINT32 vertexStride, UINT32* outIndices, UINT32 indexOffset)
	{
		UINT32 numCoords = (UINT32)points.size();

		outVertices += vertexOffset * vertexStride;
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

	RectF DrawHelper2D::normalizedCoordToClipSpace(const RectF& area) const
	{
		RectF clipSpaceRect;
		clipSpaceRect.x = area.x * 2.0f - 1.0f;
		clipSpaceRect.width = area.width * 2.0f;
		clipSpaceRect.y = -area.y * 2.0f + 1.0f;
		clipSpaceRect.height = area.height * -2.0f;

		return clipSpaceRect;
	}

	Vector2 DrawHelper2D::normalizedCoordToClipSpace(const Vector2& pos) const
	{
		return Vector2(pos.x * 2.0f - 1.0f, -pos.y * 2.0f + 1.0f);
	}
}