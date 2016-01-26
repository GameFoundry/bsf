#include "BsFBXImporter.h"
#include "BsResource.h"
#include "BsCoreApplication.h"
#include "BsDebug.h"
#include "BsDataStream.h"
#include "BsPath.h"
#include "BsMeshData.h"
#include "BsMesh.h"
#include "BsVector2.h"
#include "BsVector3.h"
#include "BsVector4.h"
#include "BsVertexDataDesc.h"

namespace BansheeEngine
{
	FBXImporter::FBXImporter()
		:SpecificImporter() 
	{
		mExtensions.push_back(L"fbx");
	}

	FBXImporter::~FBXImporter() 
	{

	}

	bool FBXImporter::isExtensionSupported(const WString& ext) const
	{
		WString lowerCaseExt = ext;
		StringUtil::toLowerCase(lowerCaseExt);

		return find(mExtensions.begin(), mExtensions.end(), lowerCaseExt) != mExtensions.end();
	}

	bool FBXImporter::isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const
	{
		return true; // FBX files can be plain-text so I don't even check for magic number
	}

	ResourcePtr FBXImporter::import(const Path& filePath, ConstImportOptionsPtr importOptions)
	{
		FbxManager* fbxManager = nullptr;
		FbxScene* fbxScene = nullptr;

		startUpSdk(fbxManager, fbxScene);
		loadScene(fbxManager, fbxScene, filePath);

		Vector<SubMesh> subMeshes;
		MeshDataPtr meshData = parseScene(fbxManager, fbxScene, subMeshes);	

		shutDownSdk(fbxManager);

		MeshPtr mesh = Mesh::_createPtr(meshData, subMeshes);

		WString fileName = filePath.getWFilename(false);
		mesh->setName(toString(fileName));

		return mesh;
	}

	void FBXImporter::startUpSdk(FbxManager*& manager, FbxScene*& scene)
	{
		// TODO Low priority - Initialize allocator methods for FBX. It calls a lot of heap allocs (200 000 calls for a simple 2k poly mesh) which slows down the import.
		// Custom allocator would help a lot.

		manager = FbxManager::Create();
		if(manager == nullptr)
			BS_EXCEPT(InternalErrorException, "FBX SDK failed to initialize. FbxManager::Create() failed.");

		FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
		manager->SetIOSettings(ios);

		scene = FbxScene::Create(manager, "Import Scene");
		if(scene == nullptr)
			BS_EXCEPT(InternalErrorException, "Failed to create FBX scene.");
	}

	void FBXImporter::shutDownSdk(FbxManager* manager)
	{
		manager->Destroy();
	}

	void FBXImporter::loadScene(FbxManager* manager, FbxScene* scene, const Path& filePath)
	{
		int lFileMajor, lFileMinor, lFileRevision;
		int lSDKMajor,  lSDKMinor,  lSDKRevision;
		FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

		FbxImporter* importer = FbxImporter::Create(manager, "");
		bool importStatus = importer->Initialize(filePath.toString().c_str(), -1, manager->GetIOSettings());
		
		importer->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

		if(!importStatus)
		{
			BS_EXCEPT(InternalErrorException, "Call to FbxImporter::Initialize() failed.\n" +
				String("Error returned: %s\n\n") + String(importer->GetStatus().GetErrorString()));
		}

		manager->GetIOSettings()->SetBoolProp(IMP_FBX_ANIMATION, false);
		manager->GetIOSettings()->SetBoolProp(IMP_FBX_TEXTURE, false);
		manager->GetIOSettings()->SetBoolProp(IMP_FBX_LINK, false);
		manager->GetIOSettings()->SetBoolProp(IMP_FBX_GOBO, false);
		manager->GetIOSettings()->SetBoolProp(IMP_FBX_SHAPE, false);

		// TODO - Parse animations
		// TODO - Parse blend shapes

		importStatus = importer->Import(scene);

		if(!importStatus)
		{
			importer->Destroy();
			
			BS_EXCEPT(InternalErrorException, "Call to FbxImporter::Initialize() failed.\n" +
				String("Error returned: %s\n\n") + String(importer->GetStatus().GetErrorString()));
		}

		importer->Destroy();
	}

	MeshDataPtr FBXImporter::parseScene(FbxManager* manager, FbxScene* scene, Vector<SubMesh>& subMeshes)
	{
		Stack<FbxNode*> todo;
		todo.push(scene->GetRootNode());

		Vector<MeshDataPtr> allMeshes;
		Vector<Vector<SubMesh>> allSubMeshes;

		while(!todo.empty())
		{
			FbxNode* curNode = todo.top();
			todo.pop();

			const char* name = curNode->GetName();

			FbxNodeAttribute* attrib = curNode->GetNodeAttribute();
			if(attrib != nullptr)
			{
				FbxNodeAttribute::EType attribType = attrib->GetAttributeType();

				switch(attribType)
				{
				case FbxNodeAttribute::eMesh:
					{
						FbxMesh* mesh = static_cast<FbxMesh*>(attrib);
						mesh->RemoveBadPolygons();

						if(!mesh->IsTriangleMesh())
						{
							FbxGeometryConverter geomConverter(manager);
							geomConverter.Triangulate(mesh, true);
							attrib = curNode->GetNodeAttribute();
							mesh = static_cast<FbxMesh*>(attrib);
						}

						allSubMeshes.push_back(Vector<SubMesh>());

						MeshDataPtr meshData = parseMesh(mesh, allSubMeshes.back()); 
						allMeshes.push_back(meshData);

						// TODO - Transform meshes based on node transform
					}
					break;
				case FbxNodeAttribute::eSkeleton:
					break; // TODO - I should probably implement skeleton parsing

				}
			}

			for(int i = 0; i < curNode->GetChildCount(); i++)
				todo.push(curNode->GetChild(i));
		}

		if(allMeshes.size() == 0)
			return nullptr;
		else if(allMeshes.size() == 1)
		{
			subMeshes = allSubMeshes[0];

			return allMeshes[0];
		}
		else
		{
			MeshDataPtr combinedMeshData = MeshData::combine(allMeshes, allSubMeshes, subMeshes);

			return combinedMeshData;
		}
	}

	MeshDataPtr FBXImporter::parseMesh(FbxMesh* mesh, Vector<SubMesh>& subMeshes, bool createTangentsIfMissing)
	{
		if (!mesh->GetNode())
		{
			VertexDataDescPtr tmpVertDesc = bs_shared_ptr<VertexDataDesc>();
			return bs_shared_ptr<MeshData, ScratchAlloc>(0, 0, tmpVertDesc);
		}

		// Find out which vertex attributes exist
		bool hasColor = mesh->GetElementVertexColorCount() > 0;
		const FbxGeometryElementVertexColor* colorElement = nullptr;
		FbxGeometryElement::EMappingMode colorMappingMode = FbxGeometryElement::eNone;
		FbxGeometryElement::EReferenceMode colorRefMode = FbxGeometryElement::eDirect;

		if(hasColor)
		{
			colorElement = mesh->GetElementVertexColor(0);
			colorMappingMode = colorElement->GetMappingMode();
			colorRefMode = colorElement->GetReferenceMode();

			if (colorMappingMode == FbxGeometryElement::eNone)
				hasColor = false;
		}

		bool hasNormal = mesh->GetElementNormalCount() > 0;
		const FbxGeometryElementNormal* normalElement = nullptr;
		FbxGeometryElement::EMappingMode normalMappingMode = FbxGeometryElement::eNone;
		FbxGeometryElement::EReferenceMode normalRefMode = FbxGeometryElement::eDirect;

		if (hasNormal)
		{
			normalElement = mesh->GetElementNormal(0);
			normalMappingMode = normalElement->GetMappingMode();
			normalRefMode = normalElement->GetReferenceMode();

			if (normalMappingMode == FbxGeometryElement::eNone)
				hasNormal = false;
		}

		bool hasTangent = mesh->GetElementTangentCount() > 0;
		const FbxGeometryElementTangent* tangentElement = nullptr;
		FbxGeometryElement::EMappingMode tangentMappingMode = FbxGeometryElement::eNone;
		FbxGeometryElement::EReferenceMode tangentRefMode = FbxGeometryElement::eDirect;

		if (hasTangent)
		{
			tangentElement = mesh->GetElementTangent(0);
			tangentMappingMode = tangentElement->GetMappingMode();
			tangentRefMode = tangentElement->GetReferenceMode();

			if (tangentMappingMode == FbxGeometryElement::eNone)
				hasTangent = false;
		}

		bool hasBitangent = mesh->GetElementBinormalCount() > 0;
		const FbxGeometryElementBinormal* bitangentElement = nullptr;
		FbxGeometryElement::EMappingMode bitangentMappingMode = FbxGeometryElement::eNone;
		FbxGeometryElement::EReferenceMode bitangentRefMode = FbxGeometryElement::eDirect;

		if (hasBitangent)
		{
			bitangentElement = mesh->GetElementBinormal(0);
			bitangentMappingMode = bitangentElement->GetMappingMode();
			bitangentRefMode = bitangentElement->GetReferenceMode();

			if (bitangentMappingMode == FbxGeometryElement::eNone)
				hasBitangent = false;
		}

		bool hasUV0 = mesh->GetElementUVCount() > 0;
		const FbxGeometryElementUV* UVElement0 = nullptr;
		FbxGeometryElement::EMappingMode UVMappingMode0 = FbxGeometryElement::eNone;
		FbxGeometryElement::EReferenceMode UVRefMode0 = FbxGeometryElement::eDirect;

		if (hasUV0)
		{
			UVElement0 = mesh->GetElementUV(0);
			UVMappingMode0 = UVElement0->GetMappingMode();
			UVRefMode0 = UVElement0->GetReferenceMode();

			if (UVMappingMode0 == FbxGeometryElement::eNone)
				hasUV0 = false;
		}

		bool hasUV1 = mesh->GetElementUVCount() > 1;
		const FbxGeometryElementUV* UVElement1 = nullptr;
		FbxGeometryElement::EMappingMode UVMappingMode1 = FbxGeometryElement::eNone;
		FbxGeometryElement::EReferenceMode UVRefMode1 = FbxGeometryElement::eDirect;

		if (hasUV1)
		{
			UVElement1 = mesh->GetElementUV(1);
			UVMappingMode1 = UVElement1->GetMappingMode();
			UVRefMode1 = UVElement1->GetReferenceMode();

			if (UVMappingMode1 == FbxGeometryElement::eNone)
				hasUV1 = false;
		}

		// Create tangents if needed
		if(createTangentsIfMissing && mesh->GetElementUVCount() > 0)
			mesh->GenerateTangentsData(0, false);

		// Calculate number of vertices and indexes
		int polygonCount = mesh->GetPolygonCount();
		UINT32 vertexCount = 0;

		VertexDataDescPtr vertexDesc = bs_shared_ptr<VertexDataDesc>();

		vertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);

		if(hasColor)
			vertexDesc->addVertElem(VET_COLOR, VES_COLOR);

		if(hasNormal)
			vertexDesc->addVertElem(VET_FLOAT3, VES_NORMAL);

		if(hasTangent)
			vertexDesc->addVertElem(VET_FLOAT3, VES_TANGENT);

		if(hasBitangent)
			vertexDesc->addVertElem(VET_FLOAT3, VES_BITANGENT);

		if (hasUV0)
			vertexDesc->addVertElem(VET_FLOAT2, VES_TEXCOORD, 0);

		if (hasUV1)
			vertexDesc->addVertElem(VET_FLOAT2, VES_TEXCOORD, 1);

		// Count the polygon count of each material
		FbxLayerElementArrayTemplate<int>* materialElementArray = NULL;
		FbxGeometryElement::EMappingMode materialMappingMode = FbxGeometryElement::eNone;

		Set<UINT32> uniqueIndices;
		UINT32 numIndices = 0;
		if (mesh->GetElementMaterial())
		{
			materialElementArray = &mesh->GetElementMaterial()->GetIndexArray();
			materialMappingMode = mesh->GetElementMaterial()->GetMappingMode();
			if (materialElementArray && materialMappingMode == FbxGeometryElement::eByPolygon)
			{
				FBX_ASSERT(materialElementArray->GetCount() == polygonCount);
				if (materialElementArray->GetCount() == polygonCount)
				{
					// Count the faces of each material
					for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
					{
						const UINT32 materialIndex = (UINT32)materialElementArray->GetAt(polygonIndex);
						if (subMeshes.size() < materialIndex + 1)
						{
							subMeshes.resize(materialIndex + 1);
						}

						subMeshes[materialIndex].indexCount += 3;
					}

					// Record the offsets and allocate index arrays
					UINT32 materialCount = (UINT32)subMeshes.size();
					int offset = 0;
					for (UINT32 i = 0; i < materialCount; ++i)
					{
						subMeshes[i].indexOffset = offset;
						offset += subMeshes[i].indexCount;

						numIndices += subMeshes[i].indexCount;
					}
					FBX_ASSERT(offset == polygonCount * 3);
				}
			}
		}

		if (subMeshes.size() == 0)
		{
			numIndices = polygonCount * 3;

			subMeshes.resize(1);
			subMeshes[0].indexCount = polygonCount * 3;
		}

		// Count number of unique vertices
		for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
		{
			for (int vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
			{
				const int controlPointIndex = mesh->GetPolygonVertex(polygonIndex, vertexIndex);
				if (uniqueIndices.find(controlPointIndex) == uniqueIndices.end())
				{
					uniqueIndices.insert(controlPointIndex);
					vertexCount++;
				}
			}
		}

		// Allocate the array memory for all vertices and indices
		MeshDataPtr meshData = bs_shared_ptr<MeshData, ScratchAlloc>(vertexCount, numIndices, vertexDesc);

		VertexElemIter<Vector3> positions = meshData->getVec3DataIter(VES_POSITION);

		VertexElemIter<UINT32> colors;
		if(hasColor)
			colors = meshData->getDWORDDataIter(VES_COLOR);

		VertexElemIter<Vector3> normals;
		if (hasNormal)
			normals = meshData->getVec3DataIter(VES_NORMAL);

		VertexElemIter<Vector3> tangents;
		if (hasTangent)
			tangents = meshData->getVec3DataIter(VES_TANGENT);

		VertexElemIter<Vector3> bitangents;
		if (hasBitangent)
			bitangents = meshData->getVec3DataIter(VES_BITANGENT);

		VertexElemIter<Vector2> uv0;
		if (hasUV0)
			uv0 = meshData->getVec2DataIter(VES_TEXCOORD, 0);

		VertexElemIter<Vector2> uv1;
		if (hasUV1)
			uv1 = meshData->getVec2DataIter(VES_TEXCOORD, 1);

		// Get node transform
		FbxAMatrix worldTransform;
		FbxAMatrix worldTransformIT;
		worldTransform = computeWorldTransform(mesh->GetNode());
		worldTransformIT = worldTransform.Inverse();
		worldTransformIT = worldTransformIT.Transpose();

		// Populate the mesh data with vertex attributes and indices
		const FbxVector4* controlPoints = mesh->GetControlPoints();

		Vector<UINT32> indexOffsetPerSubmesh;
		indexOffsetPerSubmesh.resize(subMeshes.size(), 0);

		Vector<UINT32*> indices;
		indices.resize(subMeshes.size());

		for(UINT32 i = 0; i < (UINT32)indices.size(); i++)
		{
			indices[i] = meshData->getIndices32() + subMeshes[i].indexOffset;
		}

		Map<UINT32, UINT32> indexMap;

		UINT32 curVertexCount = 0;
		for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
		{
			// The material for current face.
			int lMaterialIndex = 0;
			if (materialElementArray && materialMappingMode == FbxGeometryElement::eByPolygon)
			{
				lMaterialIndex = materialElementArray->GetAt(polygonIndex);
			}

			// Where should I save the vertex attribute index, according to the material
			int indexOffset = subMeshes[lMaterialIndex].indexOffset + indexOffsetPerSubmesh[lMaterialIndex];
			for (int vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
			{
				int controlPointIndex = mesh->GetPolygonVertex(polygonIndex, vertexIndex);
				int triangleIndex = indexOffset + (2 - vertexIndex);

				auto findIter = indexMap.find(controlPointIndex);
				if (findIter != indexMap.end())
				{
					indices[lMaterialIndex][triangleIndex] = static_cast<unsigned int>(findIter->second);
				}
				else
				{
					indices[lMaterialIndex][triangleIndex] = static_cast<unsigned int>(curVertexCount);
					FbxVector4 currentVertex = controlPoints[controlPointIndex];
					currentVertex = worldTransform.MultT(currentVertex);

					Vector3 curPosValue;
					curPosValue[0] = static_cast<float>(currentVertex[0]);
					curPosValue[1] = static_cast<float>(currentVertex[1]);
					curPosValue[2] = static_cast<float>(currentVertex[2]);

					positions.addValue(curPosValue);
					indexMap[controlPointIndex] = curVertexCount;
					++curVertexCount;

					if (hasColor)
					{
						int colorIdx = indexOffset + vertexIndex;

						if (colorMappingMode == FbxLayerElement::eByControlPoint)
							colorIdx = controlPointIndex;

						if (colorRefMode == FbxLayerElement::eIndexToDirect)
							colorIdx = colorElement->GetIndexArray().GetAt(colorIdx);

						FbxColor lCurrentColor = colorElement->GetDirectArray().GetAt(colorIdx);

						Color curColorValue;
						curColorValue[0] = static_cast<float>(lCurrentColor[0]);
						curColorValue[1] = static_cast<float>(lCurrentColor[1]);
						curColorValue[2] = static_cast<float>(lCurrentColor[2]);
						curColorValue[3] = static_cast<float>(lCurrentColor[3]);

						UINT32 color32 = curColorValue.getAsRGBA();
						colors.addValue(color32);
					}

					if (hasNormal)
					{
						int normalIdx = indexOffset + vertexIndex;

						if (normalMappingMode == FbxLayerElement::eByControlPoint)
							normalIdx = controlPointIndex;

						if (normalRefMode == FbxLayerElement::eIndexToDirect)
							normalIdx = normalElement->GetIndexArray().GetAt(normalIdx);

						FbxVector4 currentNormal = normalElement->GetDirectArray().GetAt(normalIdx);
						currentNormal = worldTransformIT.MultT(currentNormal);

						Vector3 curNormalValue;
						curNormalValue[0] = static_cast<float>(currentNormal[0]);
						curNormalValue[1] = static_cast<float>(currentNormal[1]);
						curNormalValue[2] = static_cast<float>(currentNormal[2]);

						normals.addValue(curNormalValue);
					}

					if (hasTangent)
					{
						int tangentIdx = indexOffset + vertexIndex;

						if (tangentMappingMode == FbxLayerElement::eByControlPoint)
							tangentIdx = controlPointIndex;

						if (tangentRefMode == FbxLayerElement::eIndexToDirect)
							tangentIdx = tangentElement->GetIndexArray().GetAt(tangentIdx);

						FbxVector4 currentTangent = tangentElement->GetDirectArray().GetAt(tangentIdx);
						currentTangent = worldTransformIT.MultT(currentTangent);

						Vector3 curTangentValue;
						curTangentValue[0] = static_cast<float>(currentTangent[0]);
						curTangentValue[1] = static_cast<float>(currentTangent[1]);
						curTangentValue[2] = static_cast<float>(currentTangent[2]);

						tangents.addValue(curTangentValue);
					}

					if (hasBitangent)
					{
						int bitangentIdx = indexOffset + vertexIndex;

						if (bitangentMappingMode == FbxLayerElement::eByControlPoint)
							bitangentIdx = controlPointIndex;

						if (bitangentRefMode == FbxLayerElement::eIndexToDirect)
							bitangentIdx = bitangentElement->GetIndexArray().GetAt(bitangentIdx);

						FbxVector4 currentBitangent = bitangentElement->GetDirectArray().GetAt(bitangentIdx);
						currentBitangent = worldTransformIT.MultT(currentBitangent);

						Vector3 curBitangentValue;
						curBitangentValue[0] = static_cast<float>(currentBitangent[0]);
						curBitangentValue[1] = static_cast<float>(currentBitangent[1]);
						curBitangentValue[2] = static_cast<float>(currentBitangent[2]);

						bitangents.addValue(curBitangentValue);
					}

					if (hasUV0)
					{
						int uv0Idx = indexOffset + vertexIndex;

						if (UVMappingMode0 == FbxLayerElement::eByControlPoint)
							uv0Idx = controlPointIndex;

						if (UVRefMode0 == FbxLayerElement::eIndexToDirect)
							uv0Idx = UVElement0->GetIndexArray().GetAt(uv0Idx);

						FbxVector4 currentUV = UVElement0->GetDirectArray().GetAt(uv0Idx);

						Vector2 curUV0Value;
						curUV0Value[0] = static_cast<float>(currentUV[0]);
						curUV0Value[1] = 1.0f - static_cast<float>(currentUV[1]);

						uv0.addValue(curUV0Value);
					}

					if (hasUV1)
					{
						int uv1Idx = indexOffset + vertexIndex;

						if (UVMappingMode1 == FbxLayerElement::eByControlPoint)
							uv1Idx = controlPointIndex;

						if (UVRefMode1 == FbxLayerElement::eIndexToDirect)
							uv1Idx = UVElement1->GetIndexArray().GetAt(uv1Idx);

						FbxVector4 currentUV = UVElement1->GetDirectArray().GetAt(uv1Idx);

						Vector2 curUV1Value;
						curUV1Value[0] = static_cast<float>(currentUV[0]);
						curUV1Value[1] = 1.0f - static_cast<float>(currentUV[1]);

						uv1.addValue(curUV1Value);
					}
				}
			}

			indexOffsetPerSubmesh[lMaterialIndex] += 3;
		}

		return meshData;
	}

	FbxAMatrix FBXImporter::computeWorldTransform(FbxNode* node)
	{
		FbxVector4 translation = node->GetGeometricTranslation(FbxNode::eSourcePivot);
		FbxVector4 rotation = node->GetGeometricRotation(FbxNode::eSourcePivot);
		FbxVector4 scaling = node->GetGeometricScaling(FbxNode::eSourcePivot);

		FbxAMatrix localTransform;
		localTransform.SetT(translation);
		localTransform.SetR(rotation);
		localTransform.SetS(scaling);

		FbxAMatrix& globalTransform = node->EvaluateGlobalTransform();

		FbxAMatrix worldTransform;
		worldTransform = globalTransform * localTransform;

		return worldTransform;
	}
}