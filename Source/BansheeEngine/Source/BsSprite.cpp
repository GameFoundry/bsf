//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsTextSprite.h"
#include "BsVector2.h"
#include "BsTexture.h"
#include "BsPlane.h"
#include "BsRect2.h"

namespace BansheeEngine
{
	Sprite::Sprite()
	{ }

	Sprite::~Sprite()
	{ }

	Rect2I Sprite::getBounds(const Vector2I& offset, const Rect2I& clipRect) const 
	{
		Rect2I bounds = mBounds;

		if(clipRect.width > 0 && clipRect.height > 0)
			bounds.clip(clipRect);

		bounds.x += offset.x;
		bounds.y += offset.y;

		return bounds; 
	}

	UINT32 Sprite::getNumRenderElements() const
	{
		return (UINT32)mCachedRenderElements.size();
	}

	const SpriteMaterialInfo& Sprite::getMaterialInfo(UINT32 renderElementIdx) const
	{
		return mCachedRenderElements.at(renderElementIdx).matInfo;
	}

	UINT32 Sprite::getNumQuads(UINT32 renderElementIdx) const
	{
		return mCachedRenderElements.at(renderElementIdx).numQuads;
	}

	UINT32 Sprite::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, 
		UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx, const Vector2I& offset, const Rect2I& clipRect, bool clip) const
	{
		const auto& renderElem = mCachedRenderElements.at(renderElementIdx);

		UINT32 startVert = startingQuad * 4;
		UINT32 startIndex = startingQuad * 6;

		UINT32 maxVertIdx = maxNumQuads * 4;
		UINT32 maxIndexIdx = maxNumQuads * 6;

		UINT32 mNumVertices = renderElem.numQuads * 4;
		UINT32 mNumIndices = renderElem.numQuads * 6;

		assert((startVert + mNumVertices) <= maxVertIdx);
		assert((startIndex + mNumIndices) <= maxIndexIdx);

		UINT8* vertDst = vertices + startVert * vertexStride;
		UINT8* uvDst = uv + startVert * vertexStride;

		// TODO - I'm sure this can be done in a more cache friendly way. Profile it later.
		Vector2 vecOffset((float)offset.x, (float)offset.y);
		if(clip)
		{
			for(UINT32 i = 0; i < renderElem.numQuads; i++)
			{
				UINT8* vecStart = vertDst;
				UINT8* uvStart = uvDst;
				UINT32 vertIdx = i * 4;

				memcpy(vertDst, &renderElem.vertices[vertIdx + 0], sizeof(Vector2));
				memcpy(uvDst, &renderElem.uvs[vertIdx + 0], sizeof(Vector2));

				vertDst += vertexStride;
				uvDst += vertexStride;

				memcpy(vertDst, &renderElem.vertices[vertIdx + 1], sizeof(Vector2));
				memcpy(uvDst, &renderElem.uvs[vertIdx + 1], sizeof(Vector2));

				vertDst += vertexStride;
				uvDst += vertexStride;

				memcpy(vertDst, &renderElem.vertices[vertIdx + 2], sizeof(Vector2));
				memcpy(uvDst, &renderElem.uvs[vertIdx + 2], sizeof(Vector2));

				vertDst += vertexStride;
				uvDst += vertexStride;

				memcpy(vertDst, &renderElem.vertices[vertIdx + 3], sizeof(Vector2));
				memcpy(uvDst, &renderElem.uvs[vertIdx + 3], sizeof(Vector2));

				clipQuadsToRect(vecStart, uvStart, 1, vertexStride, clipRect);

				vertDst = vecStart;
				Vector2* curVec = (Vector2*)vertDst;
				*curVec += vecOffset;

				vertDst += vertexStride;
				curVec = (Vector2*)vertDst;
				*curVec += vecOffset;

				vertDst += vertexStride;
				curVec = (Vector2*)vertDst;
				*curVec += vecOffset;

				vertDst += vertexStride;
				curVec = (Vector2*)vertDst;
				*curVec += vecOffset;

				vertDst += vertexStride;
				uvDst += vertexStride;
			}
		}
		else
		{
			for(UINT32 i = 0; i < renderElem.numQuads; i++)
			{
				UINT8* vecStart = vertDst;
				UINT32 vertIdx = i * 4;

				memcpy(vertDst, &renderElem.vertices[vertIdx + 0], sizeof(Vector2));
				memcpy(uvDst, &renderElem.uvs[vertIdx + 0], sizeof(Vector2));

				vertDst += vertexStride;
				uvDst += vertexStride;

				memcpy(vertDst, &renderElem.vertices[vertIdx + 1], sizeof(Vector2));
				memcpy(uvDst, &renderElem.uvs[vertIdx + 1], sizeof(Vector2));

				vertDst += vertexStride;
				uvDst += vertexStride;

				memcpy(vertDst, &renderElem.vertices[vertIdx + 2], sizeof(Vector2));
				memcpy(uvDst, &renderElem.uvs[vertIdx + 2], sizeof(Vector2));

				vertDst += vertexStride;
				uvDst += vertexStride;

				memcpy(vertDst, &renderElem.vertices[vertIdx + 3], sizeof(Vector2));
				memcpy(uvDst, &renderElem.uvs[vertIdx + 3], sizeof(Vector2));

				vertDst = vecStart;
				Vector2* curVec = (Vector2*)vertDst;
				*curVec += vecOffset;

				vertDst += vertexStride;
				curVec = (Vector2*)vertDst;
				*curVec += vecOffset;

				vertDst += vertexStride;
				curVec = (Vector2*)vertDst;
				*curVec += vecOffset;

				vertDst += vertexStride;
				curVec = (Vector2*)vertDst;
				*curVec += vecOffset;

				vertDst += vertexStride;
				uvDst += vertexStride;
			}
		}

		if(indices != nullptr)
			memcpy(&indices[startIndex], renderElem.indexes, mNumIndices * sizeof(UINT32));

		return renderElem.numQuads;
	}

	Vector2I Sprite::getAnchorOffset(SpriteAnchor anchor, UINT32 width, UINT32 height)
	{
		switch(anchor)
		{
		case SA_TopLeft:
			return -Vector2I(0, 0);
		case SA_TopCenter:
			return -Vector2I(width / 2, 0);
		case SA_TopRight:
			return -Vector2I(width, 0);
		case SA_MiddleLeft:
			return -Vector2I(0, height / 2);
		case SA_MiddleCenter:
			return -Vector2I(width / 2, height / 2);
		case SA_MiddleRight:
			return -Vector2I(width, height / 2);
		case SA_BottomLeft:
			return -Vector2I(0, height);
		case SA_BottomCenter:
			return -Vector2I(width / 2, height);
		case SA_BottomRight:
			return -Vector2I(width, height);
		}

		return Vector2I();
	}

	void Sprite::updateBounds() const
	{
		Vector2 min;
		Vector2 max;

		// Find starting point
		bool foundStartingPoint = false;
		for(auto& renderElem : mCachedRenderElements)
		{
			if(renderElem.vertices != nullptr && renderElem.numQuads > 0)
			{
				min = renderElem.vertices[0];
				max = renderElem.vertices[0];
				foundStartingPoint = true;
				break;
			}
		}

		if(!foundStartingPoint)
		{
			mBounds = Rect2I(0, 0, 0, 0);
			return;
		}

		// Calculate bounds
		for(auto& renderElem : mCachedRenderElements)
		{
			if(renderElem.vertices != nullptr && renderElem.numQuads > 0)
			{
				UINT32 vertexCount = renderElem.numQuads * 4;

				for(UINT32 i = 0; i < vertexCount; i++)
				{
					min = Vector2::min(min, renderElem.vertices[i]);
					max = Vector2::max(max, renderElem.vertices[i]);
				}
			}
		}

		mBounds = Rect2I((int)min.x, (int)min.y, (int)(max.x - min.x), (int)(max.y - min.y));
	}

	// This will only properly clip an array of quads
	// Vertices in the quad must be in a specific order: top left, top right, bottom left, bottom right
	// (0, 0) represents top left of the screen
	void Sprite::clipQuadsToRect(UINT8* vertices, UINT8* uv, UINT32 numQuads, UINT32 vertStride, const Rect2I& clipRect)
	{
		float left = (float)clipRect.x;
		float right = (float)clipRect.x + clipRect.width;
		float top = (float)clipRect.y;
		float bottom = (float)clipRect.y + clipRect.height;

		for(UINT32 i = 0; i < numQuads; i++)
		{
			Vector2* vecA = (Vector2*)(vertices);
			Vector2* vecB = (Vector2*)(vertices + vertStride);
			Vector2* vecC = (Vector2*)(vertices + vertStride * 2);
			Vector2* vecD = (Vector2*)(vertices + vertStride * 3);

			Vector2* uvA = (Vector2*)(uv);
			Vector2* uvB = (Vector2*)(uv + vertStride);
			Vector2* uvC = (Vector2*)(uv + vertStride * 2);
			Vector2* uvD = (Vector2*)(uv + vertStride * 3);

			// Attempt to skip those that are definitely not clipped
			if(vecA->x >= left && vecB->x <= right &&
				vecA->y >= top && vecC->y <= bottom)
			{
				continue;
			}

			// TODO - Skip those that are 100% clipped as well

			float du = (uvB->x - uvA->x) / (vecB->x - vecA->x);
			float dv = (uvA->y - uvC->y) / (vecA->y - vecD->y);

			if(right < left)
				std::swap(left, right);

			if(bottom < top)
				std::swap(bottom, top);

			// Clip left
			float newLeft = Math::clamp(vecA->x, left, right);
			float uvLeftOffset = (newLeft - vecA->x) * du;

			// Clip right
			float newRight = Math::clamp(vecB->x, left, right);
			float uvRightOffset = (vecB->x - newRight) * du;

			// Clip top
			float newTop = Math::clamp(vecA->y, top, bottom);
			float uvTopOffset = (newTop - vecA->y) * dv;

			// Clip bottom
			float newBottom = Math::clamp(vecC->y, top, bottom);
			float uvBottomOffset = (vecC->y - newBottom) * dv;

			vecA->x = newLeft;
			vecC->x = newLeft;
			vecB->x = newRight;
			vecD->x = newRight;
			vecA->y = newTop;
			vecB->y = newTop;
			vecC->y = newBottom;
			vecD->y = newBottom;
			
			uvA->x += uvLeftOffset;
			uvC->x += uvLeftOffset;
			uvB->x -= uvRightOffset;
			uvD->x -= uvRightOffset;
			uvA->y += uvTopOffset;
			uvB->y += uvTopOffset;
			uvC->y -= uvBottomOffset;
			uvD->y -= uvBottomOffset;

			vertices += vertStride * 4;
			uv += vertStride * 4;
		}
	}

	// Implementation from: http://www.geometrictools.com/Documentation/ClipMesh.pdf
	class TriangleClipper
	{
	private:
		struct ClipVert
		{
			ClipVert() { }

			Vector3 point;
			Vector2 uv;
			float distance = 0.0f;
			UINT32 occurs = 0;
			bool visible = true;
		};

		struct ClipEdge
		{
			ClipEdge() { }

			UINT32 verts[2];
			Vector<UINT32> faces;
			bool visible = true;
		};

		struct ClipFace
		{
			ClipFace() { }

			Vector<UINT32> edges;
			bool visible = true;
			Vector3 normal;
		};

		struct ClipMesh
		{
			ClipMesh() { }

			Vector<ClipVert> verts;
			Vector<ClipEdge> edges;
			Vector<ClipFace> faces;
		};

	public:
		void clip(UINT8* vertices, UINT8* uvs, UINT32 numTris, UINT32 vertexStride, const Rect2& clipRect, 
			Vector<Vector3>& clippedVertices, Vector<Vector2>& clippedUvs); // TODO - Use write callback instead of vectors for output

	private:
		INT32 clipByPlane(const Plane& plane);
		INT32 processVertices(const Plane& plane);
		void processEdges();
		void processFaces();
		void convertToMesh(Vector<Vector3>& vertices, Vector<Vector2>& uvs);
		void getOrderedFaces(FrameVector<FrameVector<UINT32>>& sortedFaces);
		void getOrderedVertices(ClipFace face, UINT32* vertices);
		Vector3 getNormal(UINT32* sortedVertices, UINT32 numVertices);
		bool getOpenPolyline(ClipFace& face, UINT32& start, UINT32& end);

		ClipMesh mesh;
	};

	void TriangleClipper::clip(UINT8* vertices, UINT8* uvs, UINT32 numTris, UINT32 vertexStride, const Rect2& clipRect,
		Vector<Vector3>& clippedVertices, Vector<Vector2>& clippedUvs)
	{
		// Add vertices
		UINT32 numVertices = numTris * 3;
		mesh.verts.resize(numVertices);

		for (UINT32 i = 0; i < numVertices; i++)
		{
			ClipVert& clipVert = mesh.verts[i];
			clipVert.point = *(Vector3*)(vertices + vertexStride * i);
			clipVert.uv = *(Vector2*)(uvs + vertexStride * i);
		}

		// Add edges & faces
		UINT32 numEdges = numTris * 3;
		mesh.edges.resize(numEdges);
		mesh.faces.resize(numTris);

		for (UINT32 i = 0; i < numTris; i++)
		{
			UINT32 idx0 = i * 3 + 0;
			UINT32 idx1 = i * 3 + 1;
			UINT32 idx2 = i * 3 + 2;

			ClipEdge& clipEdge0 = mesh.edges[idx0];
			clipEdge0.verts[0] = idx0;
			clipEdge0.verts[1] = idx1;

			ClipEdge& clipEdge1 = mesh.edges[idx1];
			clipEdge0.verts[0] = idx1;
			clipEdge0.verts[1] = idx2;

			ClipEdge& clipEdge2 = mesh.edges[idx2];
			clipEdge0.verts[0] = idx2;
			clipEdge0.verts[1] = idx0;

			ClipFace& clipFace = mesh.faces[i];

			clipFace.edges.push_back(idx0);
			clipFace.edges.push_back(idx1);
			clipFace.edges.push_back(idx2);

			clipEdge0.faces.push_back(i);
			clipEdge1.faces.push_back(i);
			clipEdge2.faces.push_back(i);

			UINT32 verts[] = { idx0, idx1, idx2, idx0 };
			for (UINT32 j = 0; j < 3; j++)
				clipFace.normal += Vector3::cross(mesh.verts[verts[j]].point, mesh.verts[verts[j + 1]].point);

			clipFace.normal.normalize();

			mesh.faces.push_back(clipFace);
		}

		Plane clipPlanes[] =
		{
			{ Vector3(1, 0, 0), clipRect.x },
			{ Vector3(-1, 0, 0), -(clipRect.x + clipRect.width) },
			{ Vector3(0, 1, 0), (clipRect.y + clipRect.height) },
			{ Vector3(0, -1, 0), -clipRect.y }
		};

		for (int i = 0; i < 4; i++)
			clipByPlane(clipPlanes[i]);

		convertToMesh(clippedVertices, clippedUvs);
	}

	INT32 TriangleClipper::clipByPlane(const Plane& plane)
	{
		int state = processVertices(plane);

		if (state == 1)
			return +1; // Nothing is clipped
		else if (state == -1)
			return -1; // Everything is clipped

		processEdges();
		processFaces();

		return 0;
	}

	INT32 TriangleClipper::processVertices(const Plane& plane)
	{
		static const float EPSILON = 0.00001f;

		// Compute signed distances from vertices to plane
		int positive = 0, negative = 0;
		for (UINT32 i = 0; i < (UINT32)mesh.verts.size(); i++)
		{
			ClipVert& vertex = mesh.verts[i];

			if (vertex.visible)
			{
				vertex.distance = Vector3::dot(plane.normal, vertex.point) - plane.d;
				if (vertex.distance >= EPSILON)
				{
					positive++;
				}
				else if (vertex.distance <= -EPSILON)
				{
					negative++;
					vertex.visible = false;
				}
				else
				{
					// Point on the plane within floating point tolerance
					vertex.distance = 0;
				}
			}
		}
		if (negative == 0)
		{
			// All vertices on nonnegative side, no clipping
			return +1;
		}
		if (positive == 0)
		{
			// All vertices on nonpositive side, everything clipped
			return -1;
		}

		return 0;
	}

	void TriangleClipper::processEdges()
	{
		for (INT32 i = 0; i < (UINT32)mesh.edges.size(); i++)
		{
			ClipEdge& edge = mesh.edges[i];

			if (edge.visible)
			{
				const ClipVert& v0 = mesh.verts[edge.verts[0]];
				const ClipVert& v1 = mesh.verts[edge.verts[1]];

				float d0 = v0.distance;
				float d1 = v1.distance;

				if (d0 <= 0 && d1 <= 0)
				{
					// Edge is culled, remove edge from faces sharing it
					for (UINT32 j = 0; j < (UINT32)edge.faces.size(); j++)
					{
						ClipFace& face = mesh.faces[edge.faces[j]];

						auto iterFind = std::find(face.edges.begin(), face.edges.end(), i);
						if (iterFind != face.edges.end())
						{
							face.edges.erase(iterFind);

							if (face.edges.empty())
								face.visible = false;
						}
					}

					edge.visible = false;
					continue;
				}

				if (d0 >= 0 && d1 >= 0)
				{
					// Edge is on nonnegative side, faces retain the edge
					continue;
				}

				// The edge is split by the plane. Compute the point of intersection.
				// If the old edge is <V0,V1> and I is the intersection point, the new
				// edge is <V0,I> when d0 > 0 or <I,V1> when d1 > 0.
				float t = d0 / (d0 - d1);
				Vector3 intersectPt = (1 - t)*v0.point + t*v1.point;
				Vector2 intersectUv = (1 - t)*v0.uv + t*v1.uv;

				UINT32 newVertIdx = (UINT32)mesh.verts.size();
				mesh.verts.push_back(ClipVert());

				ClipVert& newVert = mesh.verts.back();
				newVert.point = intersectPt;
				newVert.uv = intersectUv;

				if (d0 > 0)
					mesh.edges[i].verts[1] = newVertIdx;
				else
					mesh.edges[i].verts[0] = newVertIdx;
			}
		}
	}

	void TriangleClipper::processFaces()
	{
		for (UINT32 i = 0; i < (UINT32)mesh.faces.size(); i++)
		{
			ClipFace& face = mesh.faces[i];

			if (face.visible)
			{
				// The edge is culled. If the edge is exactly on the clip
				// plane, it is possible that a visible triangle shares it.
				// The edge will be re-added during the face loop.

				for (UINT32 j = 0; j < (UINT32)face.edges.size(); j++)
				{
					ClipEdge& edge = mesh.edges[face.edges[j]];
					ClipVert& v0 = mesh.verts[edge.verts[0]];
					ClipVert& v1 = mesh.verts[edge.verts[0]];

					v0.occurs = 0;
					v1.occurs = 0;
				}
			}

			UINT32 start, end;
			if (getOpenPolyline(mesh.faces[i], start, end))
			{
				// Polyline is open, close it
				UINT32 closeEdgeIdx = (UINT32)mesh.edges.size();
				mesh.edges.push_back(ClipEdge());
				ClipEdge& closeEdge = mesh.edges.back();

				closeEdge.verts[0] = start;
				closeEdge.verts[1] = end;

				closeEdge.faces.push_back(i);
				face.edges.push_back(closeEdgeIdx);
			}
		}
	}

	bool TriangleClipper::getOpenPolyline(ClipFace& face, UINT32& start, UINT32& end)
	{
		// Count the number of occurrences of each vertex in the polyline. The
		// resulting "occurs" values must be 1 or 2.
		for (UINT32 i = 0; i < (UINT32)face.edges.size(); i++)
		{
			ClipEdge& edge = mesh.edges[face.edges[i]];
	
			if (edge.visible)
			{
				ClipVert& v0 = mesh.verts[edge.verts[0]];
				ClipVert& v1 = mesh.verts[edge.verts[0]];

				v0.occurs++;
				v1.occurs++;
			}
		}

		// Determine if the polyline is open
		bool gotStart = false;
		bool gotEnd = false;
		for (UINT32 i = 0; i < (UINT32)face.edges.size(); i++)
		{
			const ClipEdge& edge = mesh.edges[face.edges[i]];

			const ClipVert& v0 = mesh.verts[edge.verts[0]];
			const ClipVert& v1 = mesh.verts[edge.verts[1]];

			if (v0.occurs == 1)
			{
				if (!gotStart)
				{
					start = edge.verts[0];
					gotStart = true;
				}
				else if (!gotEnd)
				{
					end = edge.verts[0];
					gotEnd = true;
				}
			}

			if (v1.occurs == 1)
			{
				if (!gotStart)
				{
					start = edge.verts[1];
					gotStart = true;
				}
				else if (!gotEnd)
				{
					end = edge.verts[1];
					gotEnd = true;
				}
			}
		}

		return gotStart;
	}

	void TriangleClipper::convertToMesh(Vector<Vector3>& vertices, Vector<Vector2>& uvs)
	{
		bs_frame_mark();
		{
			FrameVector<FrameVector<UINT32>> allFaces;
			getOrderedFaces(allFaces);

			// Note: Consider using Delaunay triangulation to avoid skinny triangles
			for (auto& face : allFaces)
			{
				for (int i = 0; i < (UINT32)face.size() - 2; i++)
				{
					vertices.push_back(mesh.verts[face[0]].point);
					vertices.push_back(mesh.verts[face[i + 1]].point);
					vertices.push_back(mesh.verts[face[i + 2]].point);

					uvs.push_back(mesh.verts[face[0]].uv);
					uvs.push_back(mesh.verts[face[i + 1]].uv);
					uvs.push_back(mesh.verts[face[i + 2]].uv);
				}
			}
		}
		bs_frame_clear();
	}

	void TriangleClipper::getOrderedFaces(FrameVector<FrameVector<UINT32>>& sortedFaces)
	{
		for (UINT32 i = 0; i < (UINT32)mesh.faces.size(); i++)
		{
			const ClipFace& face = mesh.faces[i];

			if (face.visible)
			{
				// Get the ordered vertices of the face. The first and last
				// element of the array are the same since the polyline is
				// closed.
				UINT32 numSortedVerts = (UINT32)face.edges.size() + 1;
				UINT32* sortedVerts = (UINT32*)bs_stack_alloc(sizeof(UINT32) * numSortedVerts);

				getOrderedVertices(face, sortedVerts);

				FrameVector<UINT32> faceVerts;

				// The convention is that the vertices should be counterclockwise
				// ordered when viewed from the negative side of the plane of the
				// face. If you need the opposite convention, switch the
				// inequality in the if-else statement.
				Vector3 normal = getNormal(sortedVerts, numSortedVerts);
				if (Vector3::dot(mesh.faces[i].normal, normal) < 0)
				{
					// Clockwise, need to swap
					for (INT32 j = (INT32)numSortedVerts - 2; j >= 0; j--)
						faceVerts.push_back(sortedVerts[j]);
				}
				else
				{
					// Counterclockwise
					for (int j = 0; j <= (INT32)numSortedVerts - 2; j++)
						faceVerts.push_back(sortedVerts[j]);
				}

				sortedFaces.push_back(faceVerts);
				bs_stack_free(sortedVerts);
			}
		}
	}

	void TriangleClipper::getOrderedVertices(ClipFace face, UINT32* sortedVerts)
	{
		UINT32 numEdges = (UINT32)face.edges.size();
		UINT32* sortedEdges = (UINT32*)bs_stack_alloc(sizeof(UINT32) * numEdges);
		for (UINT32 i = 0; i < numEdges; i++)
			sortedEdges[i] = i;

		// Bubble sort to arrange edges in contiguous order
		for (UINT32 i0 = 0, i1 = 1, choice = 1; i1 < numEdges - 1; i0 = i1, i1++)
		{
			const ClipEdge& edge0 = mesh.edges[sortedEdges[i0]];

			UINT32 current = edge0.verts[choice];
			for (UINT32 j = i1; j < numEdges; j++)
			{
				const ClipEdge& edge1 = mesh.edges[sortedEdges[j]];

				if (edge1.verts[0] == current || edge1.verts[1] == current)
				{
					std::swap(sortedEdges[i1], sortedEdges[j]);
					choice = 1;
					break;
				}
			}
		}

		// Add the first two vertices
		sortedVerts[0] = mesh.edges[sortedEdges[0]].verts[0];
		sortedVerts[1] = mesh.edges[sortedEdges[0]].verts[1];

		// Add the remaining vertices
		for (UINT32 i = 1; i < numEdges; i++)
		{
			const ClipEdge& edge = mesh.edges[sortedEdges[i]];

			if (edge.verts[0] == sortedVerts[i])
				sortedVerts[i + 1] = edge.verts[1];
			else
				sortedVerts[i + 1] = edge.verts[0];
		}

		bs_stack_free(sortedEdges);
	}

	Vector3 TriangleClipper::getNormal(UINT32* sortedVertices, UINT32 numVertices)
	{
		Vector3 normal;
		for (UINT32 i = 0; i <= numVertices - 2; i++)
			normal += Vector3::cross(mesh.verts[sortedVertices[i]].point, mesh.verts[sortedVertices[i + 1]].point);

		normal.normalize();
		return normal;
	}

	void Sprite::clipTrianglesToRect(UINT8* vertices, UINT8* uv, UINT32 numTris, UINT32 vertStride, const Rect2I& clipRect)
	{
		
	}

	UINT64 SpriteMaterialInfo::generateHash() const
	{
		UINT64 textureId = 0;
		if (texture.isLoaded())
			textureId = texture->getInternalID();

		size_t hash = 0;
		hash_combine(hash, groupId);
		hash_combine(hash, type);
		hash_combine(hash, textureId);
		hash_combine(hash, tint);

		return (UINT64)hash;
	}

	bool operator==(const SpriteMaterialInfo& lhs, const SpriteMaterialInfo& rhs)
	{
		return lhs.groupId == rhs.groupId && lhs.texture == rhs.texture && lhs.type == rhs.type && lhs.tint == rhs.tint;
	}

	bool operator!=(const SpriteMaterialInfo& lhs, const SpriteMaterialInfo& rhs)
	{
		return !(lhs == rhs);
	}
}