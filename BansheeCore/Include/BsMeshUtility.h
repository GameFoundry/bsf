#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine
{
	/** @addtogroup Utility-Core
	 *  @{
	 */

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
		 * Vertices should be split before calling this method if there are any discontinuities. (e.g. a vertex on a corner
		 * of a cube should be split into three vertices used by three triangles in order for the normals to be valid.)
		 */
		static void calculateNormals(Vector3* vertices, UINT8* indices, UINT32 numVertices, 
			UINT32 numIndices, Vector3* normals, UINT32 indexSize = 4);

		/**
		 * Calculates per-vertex tangents and bitangents based on the provided vertices, uv coordinates and indices.
		 *
		 * @param[in]	vertices	Set of vertices containing vertex positions.
		 * @param[in]	normals		Set of normals to use when calculating tangents. Must the the same length as the number
		 *							of vertices.
		 * @param[in]	uv			Set of UV coordinates to use when calculating tangents. Must the the same length as the
		 *							number of vertices.
		 * @param[in]	indices		Set of indices containing indexes into vertex array for each triangle.
		 * @param[in]	numVertices	Number of vertices in the @p vertices, @p normals and @p uv arrays.
		 * @param[in]	numIndices	Number of indices in the @p indices array. Must be a multiple of three.
		 * @param[out]	tangents	Pre-allocated buffer that will contain the calculated tangents. Must be the same size
		 *							as the vertex array.
		 * @param[out]	bitangents	Pre-allocated buffer that will contain the calculated bitangents. Must be the same size
		 *							as the vertex array.
		 * @param[in]	indexSize	Size of a single index in the indices array, in bytes.
		 *
		 * @note	
		 * Vertices should be split before calling this method if there are any discontinuities. (e.g. a vertex on a corner
		 * of a cube should be split into three vertices used by three triangles in order for the normals to be valid.)
		 */
		static void calculateTangents(Vector3* vertices, Vector3* normals, Vector2* uv, UINT8* indices, UINT32 numVertices, 
			UINT32 numIndices, Vector3* tangents, Vector3* bitangents, UINT32 indexSize = 4);

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
		 * Vertices should be split before calling this method if there are any discontinuities. (e.g. a vertex on a corner
		 * of a cube should be split into three vertices used by three triangles in order for the normals to be valid.)
		 */
		static void calculateTangentSpace(Vector3* vertices, Vector2* uv, UINT8* indices, UINT32 numVertices, 
			UINT32 numIndices, Vector3* normals, Vector3* tangents, Vector3* bitangents, UINT32 indexSize = 4);
	};

	/** @} */
}