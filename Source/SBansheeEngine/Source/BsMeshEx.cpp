//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsMeshEx.h"
#include "BsCoreThread.h"

namespace bs
{
	HMesh MeshEx::create(int numVertices, int numIndices, DrawOperationType topology, MeshUsage usage, 
		VertexLayout vertex, IndexType index)
	{
		MESH_DESC desc;
		desc.numVertices = numVertices;
		desc.numIndices = numIndices;
		desc.vertexDesc = RendererMeshData::vertexLayoutVertexDesc(vertex);
		desc.subMeshes = { SubMesh(0, numIndices, topology) };
		desc.usage = usage;
		desc.indexType = index;

		return Mesh::create(desc);
	}

	HMesh MeshEx::create(int numVertices, int numIndices, const Vector<SubMesh>& subMeshes, MeshUsage usage,
		VertexLayout vertex, IndexType index)
	{
		MESH_DESC desc;
		desc.numVertices = numVertices;
		desc.numIndices = numIndices;
		desc.vertexDesc = RendererMeshData::vertexLayoutVertexDesc(vertex);
		desc.subMeshes = subMeshes;
		desc.usage = usage;
		desc.indexType = index;

		return Mesh::create(desc);
	}

	HMesh MeshEx::create(const SPtr<RendererMeshData>& data, DrawOperationType topology, MeshUsage usage)
	{
		SPtr<MeshData> meshData;
		if (data != nullptr)
			meshData = data->getData();

		UINT32 numIndices = 0;
		if (meshData != nullptr)
			numIndices = meshData->getNumIndices();

		MESH_DESC desc;
		desc.subMeshes = { SubMesh(0, numIndices, topology) };
		desc.usage = usage;

		return Mesh::create(meshData, desc);
	}

	HMesh MeshEx::create(const SPtr<RendererMeshData>& data, const Vector<SubMesh>& subMeshes, MeshUsage usage)
	{
		SPtr<MeshData> meshData;
		if (data != nullptr)
			meshData = data->getData();

		MESH_DESC desc;
		desc.subMeshes = subMeshes;
		desc.usage = usage;

		return Mesh::create(meshData, desc);
	}

	Vector<SubMesh> MeshEx::getSubMeshes(const HMesh& thisPtr)
	{
		UINT32 numSubMeshes = thisPtr->getProperties().getNumSubMeshes();
		Vector<SubMesh> output(numSubMeshes);
		for (UINT32 i = 0; i < numSubMeshes; i++)
			output[i] = thisPtr->getProperties().getSubMesh(i);

		return output;
	}

	UINT32 MeshEx::getSubMeshCount(const HMesh& thisPtr)
	{
		return thisPtr->getProperties().getNumSubMeshes();
	}

	void MeshEx::getBounds(const HMesh& thisPtr, AABox* box, Sphere* sphere)
	{
		Bounds bounds = thisPtr->getProperties().getBounds();
		*box = bounds.getBox();
		*sphere = bounds.getSphere();
	}

	SPtr<RendererMeshData> MeshEx::getMeshData(const HMesh& thisPtr)
	{
		SPtr<MeshData> meshData = thisPtr->allocBuffer();
		thisPtr->readCachedData(*meshData);

		return RendererMeshData::create(meshData);
	}

	void MeshEx::setMeshData(const HMesh& thisPtr, const SPtr<RendererMeshData>& value)
	{
		if (value != nullptr)
		{
			SPtr<MeshData> meshData = value->getData();
			thisPtr->writeData(meshData, true);
		}
	}
}