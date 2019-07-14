//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Math/BsVector3.h"

namespace bs
{
	/** @addtogroup Utility-Core
	 *  @{
	 */

	/** Normal packed in a 32-bit structure. */
	union PackedNormal
	{
		struct
		{
			UINT8 x;
			UINT8 y;
			UINT8 z;
			UINT8 w;
		};

		UINT32 packed;
	};

	/** Performs various operations on mesh geometry. */
	class BS_CORE_EXPORT MeshUtility
	{
	public:
		/**
		 * Calculates per-vertex normals based on the provided vertices and indices.
		 *
		 * @param[in]	vertices	Set of vertices containing vertex positions.
		 * @param[in]	indices		Set of indices containing indexes into vertex array for each triangle.
		 * @param[in]	numVertices	Number of vertices in the @p vertices array.
		 * @param[in]	numIndices	Number of indices in the @p indices array. Must be a multiple of three.
		 * @param[out]	normals		Pre-allocated buffer that will contain the calculated normals. Must be the same size
		 *							as the vertex array.
		 * @param[in]	indexSize	Size of a single index in the indices array, in bytes.
		 *
		 * @note	
		 * Vertices should be split before calling this method if there are any discontinuities. (for example a vertex on a
		 * corner of a cube should be split into three vertices used by three triangles in order for the normals to be
		 * valid.)
		 */
		static void calculateNormals(Vector3* vertices, UINT8* indices, UINT32 numVertices,
			UINT32 numIndices, Vector3* normals, UINT32 indexSize = 4);

		/**
		 * Calculates per-vertex tangents and bitangents based on the provided vertices, uv coordinates and indices.
		 *
		 * @param[in]	vertices		Set of vertices containing vertex positions.
		 * @param[in]	normals			Set of normals to use when calculating tangents. Must the the same length as the
		 *								number of vertices.
		 * @param[in]	uv				Set of UV coordinates to use when calculating tangents. Must the the same length as
		 *								the number of vertices.
		 * @param[in]	indices			Set of indices containing indexes into vertex array for each triangle.
		 * @param[in]	numVertices		Number of vertices in the @p vertices, @p normals and @p uv arrays.
		 * @param[in]	numIndices		Number of indices in the @p indices array. Must be a multiple of three.
		 * @param[out]	tangents		Pre-allocated buffer that will contain the calculated tangents. Must be the same
		 *								size as the vertex array.
		 * @param[out]	bitangents		Pre-allocated buffer that will contain the calculated bitangents. Must be the same
		 *								size as the vertex array.
		 * @param[in]	indexSize		Size of a single index in the indices array, in bytes.
		 * @param[in]	vertexStride	Number of bytes to advance the @p vertices, @p normals and @p uv arrays with each
		 *								vertex. If set to zero them each array is advanced according to its own size.
		 *
		 * @note	
		 * Vertices should be split before calling this method if there are any discontinuities. (for example a vertex on a
		 * corner of a cube should be split into three vertices used by three triangles in order for the normals to be
		 * valid.)
		 */
		static void calculateTangents(Vector3* vertices, Vector3* normals, Vector2* uv, UINT8* indices, UINT32 numVertices,
			UINT32 numIndices, Vector3* tangents, Vector3* bitangents, UINT32 indexSize = 4, UINT32 vertexStride = 0);

		/**
		 * Calculates per-vertex tangent space (normal, tangent, bitangent) based on the provided vertices, uv coordinates
		 * and indices.
		 *
		 * @param[in]	vertices	Set of vertices containing vertex positions.
		 * @param[in]	uv			Set of UV coordinates to use when calculating tangents.
		 * @param[in]	indices		Set of indices containing indexes into vertex array for each triangle.
		 * @param[in]	numVertices	Number of vertices in the "vertices" array.
		 * @param[in]	numIndices	Number of indices in the "indices" array. Must be a multiple of three.
		 * @param[out]	normals		Pre-allocated buffer that will contain the calculated normals. Must be the same size
		 *							as the vertex array.
		 * @param[out]	tangents	Pre-allocated buffer that will contain the calculated tangents. Must be the same size
		 *							as the vertex array.
		 * @param[out]	bitangents	Pre-allocated buffer that will contain the calculated bitangents. Must be the same size
		 *							as the vertex array.
		 * @param[in]	indexSize	Size of a single index in the indices array, in bytes.
		 *
		 * @note	
		 * Vertices should be split before calling this method if there are any discontinuities. (for example. a vertex on
		 * a corner of a cube should be split into three vertices used by three triangles in order for the normals to be
		 * valid.)
		 */
		static void calculateTangentSpace(Vector3* vertices, Vector2* uv, UINT8* indices, UINT32 numVertices,
			UINT32 numIndices, Vector3* normals, Vector3* tangents, Vector3* bitangents, UINT32 indexSize = 4);

		/**
		 * Clips a set of two-dimensional vertices and uv coordinates against a set of arbitrary planes.
		 *
		 * @param[in]	vertices			A set of vertices in Vector2 format. Each vertex should be @p vertexStride bytes
		 *									from each other.
		 * @param[in]	uvs					A set of UV coordinates in Vector2 format. Each coordinate should be
		 *									@p vertexStride bytes from each other. Can be null if UV is not needed.
		 * @param[in]	numTris				Number of triangles to clip (must be number of vertices/uvs / 3).
		 * @param[in]	vertexStride		Distance in bytes between two separate vertex or UV values in the provided
		 *									@p vertices and @p uvs buffers.
		 * @param[in]	clipPlanes			A set of planes to clip the vertices against. Since the vertices are
		 *									two-dimensional the plane's Z coordinate should be zero.
		 * @param[in]	writeCallback		Callback that will be triggered when clipped vertices and UV coordinates are
		 *									generated and need to be stored. Vertices are always generate in tuples of
		 *									three, forming a single triangle.
		 */
		static void clip2D(UINT8* vertices, UINT8* uvs, UINT32 numTris, UINT32 vertexStride, const Vector<Plane>& clipPlanes,
			const std::function<void(Vector2*, Vector2*, UINT32)>& writeCallback);

		/**
		 * Clips a set of three-dimensional vertices and uv coordinates against a set of arbitrary planes.
		 *
		 * @param[in]	vertices			A set of vertices in Vector3 format. Each vertex should be @p vertexStride bytes
		 *									from each other.
		 * @param[in]	uvs					A set of UV coordinates in Vector2 format. Each coordinate should be
		 *									@p vertexStride bytes from each other. Can be null if UV is not needed.
		 * @param[in]	numTris				Number of triangles to clip (must be number of vertices/uvs / 3).
		 * @param[in]	vertexStride		Distance in bytes between two separate vertex or UV values in the provided
		 *									@p vertices and @p uvs buffers.
		 * @param[in]	clipPlanes			A set of planes to clip the vertices against.
		 * @param[in]	writeCallback		Callback that will be triggered when clipped vertices and UV coordinates are
		 *									generated and need to be stored. Vertices are always generate in tuples of
		 *									three, forming a single triangle.
		 */
		static void clip3D(UINT8* vertices, UINT8* uvs, UINT32 numTris, UINT32 vertexStride, const Vector<Plane>& clipPlanes,
			const std::function<void(Vector3*, Vector2*, UINT32)>& writeCallback);

		/**
		 * Encodes normals from 32-bit float format into 4D 8-bit packed format.
		 *
		 * @param[in]	source			Buffer containing data to encode. Must have @p count entries.
		 * @param[out]	destination		Buffer to output the data to. Must have @p count entries, each 32-bits.
		 * @param[in]	count			Number of entries in the @p source and @p destination arrays.
		 * @param[in]	inStride		Distance between two entries in the @p source buffer, in bytes.
		 * @param[in]	outStride		Distance between two entries in the @p destination buffer, in bytes.
		 */
		static void packNormals(Vector3* source, UINT8* destination, UINT32 count, UINT32 inStride, UINT32 outStride);

		/**
		 * Encodes normals from 32-bit float format into 4D 8-bit packed format.
		 *
		 * @param[in]	source			Buffer containing data to encode. Must have @p count entries.
		 * @param[out]	destination		Buffer to output the data to. Must have @p count entries, each 32-bits.
		 * @param[in]	count			Number of entries in the @p source and @p destination arrays.
		 * @param[in]	inStride		Distance between two entries in the @p source buffer, in bytes.
		 * @param[in]	outStride		Distance between two entries in the @p destination buffer, in bytes.
		 */
		static void packNormals(Vector4* source, UINT8* destination, UINT32 count, UINT32 inStride, UINT32 outStride);

		/**
		 * Decodes normals from 4D 8-bit packed format into a 32-bit float format.
		 *
		 * @param[in]	source			Buffer containing data to encode. Must have @p count entries, each 32-bits.
		 * @param[out]	destination		Buffer to output the data to. Must have @p count entries.
		 * @param[in]	count			Number of entries in the @p source and @p destination arrays.
		 * @param[in]	stride			Distance between two entries in the @p source buffer, in bytes.
		 */
		static void unpackNormals(UINT8* source, Vector3* destination, UINT32 count, UINT32 stride);

		/**
		 * Decodes normals from 4D 8-bit packed format into a 32-bit float format.
		 *
		 * @param[in]	source			Buffer containing data to encode. Must have @p count entries, each 32-bits.
		 * @param[out]	destination		Buffer to output the data to. Must have @p count entries.
		 * @param[in]	count			Number of entries in the @p source and @p destination arrays.
		 * @param[in]	stride			Distance between two entries in the @p source buffer, in bytes.
		 */
		static void unpackNormals(UINT8* source, Vector4* destination, UINT32 count, UINT32 stride);

		/** Decodes a normal from 4D 8-bit packed format into a 32-bit float format. */
		static Vector3 unpackNormal(const UINT8* source)
		{
			const PackedNormal& packed = *(PackedNormal*)source;
			Vector3 output;

			const float inv = (1.0f / 255.0f) * 2.0f;
			output.x = (packed.x * inv - 1.0f);
			output.y = (packed.y * inv - 1.0f);
			output.z = (packed.z * inv - 1.0f);

			return output;
		}
	};

	/** @} */
}
