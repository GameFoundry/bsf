//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsMeshUtility.h"
#include "BsVector3.h"
#include "BsVector2.h"

namespace BansheeEngine
{
	struct VertexFaces
	{
		UINT32* faces;
		UINT32 numFaces = 0;
	};

	struct VertexConnectivity
	{
		VertexConnectivity(UINT8* indices, UINT32 numVertices, UINT32 numFaces, UINT32 indexSize)
			:vertexFaces(nullptr), mMaxFacesPerVertex(0), mFaces(nullptr), mNumVertices(numVertices)
		{
			vertexFaces = bs_newN<VertexFaces>(numVertices);

			resizeFaceArray(10);

			for (UINT32 i = 0; i < numFaces; i++)
			{
				for (UINT32 j = 0; j < 3; j++)
				{
					UINT32 idx = i * 3 + j;
					UINT32 vertexIdx = 0;
					memcpy(&vertexIdx, indices + idx * indexSize, indexSize);

					assert(vertexIdx < mNumVertices);
					VertexFaces& faces = vertexFaces[vertexIdx];
					if (faces.numFaces >= mMaxFacesPerVertex)
						resizeFaceArray(mMaxFacesPerVertex * 2);

					faces.faces[faces.numFaces] = i;
					faces.numFaces++;
				}
			}
		}

		~VertexConnectivity()
		{
			if (vertexFaces != nullptr)
				bs_deleteN(vertexFaces, mNumVertices);

			if (mFaces != nullptr)
				bs_free(mFaces);
		}

		VertexFaces* vertexFaces;

	private:
		void resizeFaceArray(UINT32 numFaces)
		{
			UINT32* newFaces = (UINT32*)bs_alloc(numFaces * mNumVertices * sizeof(UINT32));

			if (mFaces != nullptr)
			{
				for (UINT32 i = 0; i < mNumVertices; i++)
					memcpy(newFaces + (i * numFaces), mFaces + (i * mMaxFacesPerVertex), mMaxFacesPerVertex * sizeof(UINT32));

				bs_free(mFaces);
			}

			for (UINT32 i = 0; i < mNumVertices; i++)
				vertexFaces[i].faces = newFaces + (i * numFaces);

			mFaces = newFaces;
			mMaxFacesPerVertex = numFaces;
		}

		UINT32 mMaxFacesPerVertex;
		UINT32 mNumVertices;
		UINT32* mFaces;
	};

	void MeshUtility::calculateNormals(Vector3* vertices, UINT8* indices, UINT32 numVertices,
		UINT32 numIndices, Vector3* normals, UINT32 indexSize)
	{
		UINT32 numFaces = numIndices / 3;

		Vector3* faceNormals = bs_newN<Vector3>(numFaces);
		for (UINT32 i = 0; i < numFaces; i++)
		{
			UINT32 triangle[3];
			memcpy(&triangle[0], indices + (i * 3 + 0) * indexSize, indexSize);
			memcpy(&triangle[1], indices + (i * 3 + 1) * indexSize, indexSize);
			memcpy(&triangle[2], indices + (i * 3 + 2) * indexSize, indexSize);

			Vector3 edgeA = vertices[triangle[1]] - vertices[triangle[0]];
			Vector3 edgeB = vertices[triangle[2]] - vertices[triangle[0]];
			faceNormals[i] = Vector3::normalize(Vector3::cross(edgeA, edgeB));

			// Note: Potentially don't normalize here in order to weigh the normals
			// by triangle size
		}

		VertexConnectivity connectivity(indices, numVertices, numFaces, indexSize);
		for (UINT32 i = 0; i < numVertices; i++)
		{
			VertexFaces& faces = connectivity.vertexFaces[i];

			for (UINT32 j = 0; j < faces.numFaces; j++)
			{
				UINT32 faceIdx = faces.faces[j];
				normals[i] += faceNormals[faceIdx];
			}

			normals[i].normalize();
		}

		bs_deleteN(faceNormals, numFaces);
	}

	void MeshUtility::calculateTangents(Vector3* vertices, Vector3* normals, Vector2* uv, UINT8* indices, UINT32 numVertices,
		UINT32 numIndices, Vector3* tangents, Vector3* bitangents, UINT32 indexSize)
	{
		UINT32 numFaces = numIndices / 3;

		Vector3* faceTangents = bs_newN<Vector3>(numFaces);
		Vector3* faceBitangents = bs_newN<Vector3>(numFaces);
		for (UINT32 i = 0; i < numFaces; i++)
		{
			UINT32 triangle[3];
			memcpy(&triangle[0], indices + (i * 3 + 0) * indexSize, indexSize);
			memcpy(&triangle[1], indices + (i * 3 + 1) * indexSize, indexSize);
			memcpy(&triangle[2], indices + (i * 3 + 2) * indexSize, indexSize);

			Vector3 p0 = vertices[triangle[0]];
			Vector3 p1 = vertices[triangle[1]];
			Vector3 p2 = vertices[triangle[2]];

			Vector2 uv0 = uv[triangle[0]];
			Vector2 uv1 = uv[triangle[1]];
			Vector2 uv2 = uv[triangle[2]];

			Vector3 q0 = p1 - p0;
			Vector3 q1 = p2 - p0;

			Vector2 s;
			s.x = uv1.x - uv0.x;
			s.y = uv2.x - uv0.x;

			Vector2 t;
			t.x = uv1.y - uv0.y;
			t.y = uv2.y - uv0.y;

			float denom = s.x*t.y - s.y * t.x;
			if (fabs(denom) >= 0e-8f)
			{
				float r = 1.0f / denom;
				s *= r;
				t *= r;

				faceTangents[i] = t.y * q0 - t.x * q1;
				faceBitangents[i] = s.x * q0 - s.y * q1;

				faceTangents[i].normalize();
				faceBitangents[i].normalize();
			}

			// Note: Potentially don't normalize here in order to weigh the normals
			// by triangle size
		}

		VertexConnectivity connectivity(indices, numVertices, numFaces, indexSize);
		for (UINT32 i = 0; i < numVertices; i++)
		{
			VertexFaces& faces = connectivity.vertexFaces[i];

			for (UINT32 j = 0; j < faces.numFaces; j++)
			{
				UINT32 faceIdx = faces.faces[j];
				tangents[i] += faceTangents[faceIdx];
				bitangents[i] += faceBitangents[faceIdx];
			}

			tangents[i].normalize();
			bitangents[i].normalize();

			// Orthonormalize
			float dot0 = normals[i].dot(tangents[i]);
			tangents[i] -= dot0*normals[i];
			tangents[i].normalize();

			float dot1 = tangents[i].dot(bitangents[i]);
			dot0 = normals[i].dot(bitangents[i]);
			bitangents[i] -= dot0*normals[i] + dot1*tangents[i];
			bitangents[i].normalize();
		}

		bs_deleteN(faceTangents, numFaces);
		bs_deleteN(faceBitangents, numFaces);

		// TODO - Consider weighing tangents by triangle size and/or edge angles
	}

	void MeshUtility::calculateTangentSpace(Vector3* vertices, Vector2* uv, UINT8* indices, UINT32 numVertices,
		UINT32 numIndices, Vector3* normals, Vector3* tangents, Vector3* bitangents, UINT32 indexSize)
	{
		calculateNormals(vertices, indices, numVertices, numIndices, normals, indexSize);
		calculateTangents(vertices, normals, uv, indices, numVertices, numIndices, tangents, bitangents, indexSize);
	}
}