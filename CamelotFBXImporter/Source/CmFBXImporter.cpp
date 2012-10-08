#include "CmFBXImporter.h"
#include "CmResource.h"
#include "CmDebug.h"
#include "CmDataStream.h"
#include "CmPath.h"
#include "CmMeshData.h"
#include "CmVector2.h"
#include "CmVector3.h"
#include "CmVector4.h"

namespace CamelotEngine
{
	FBXImporter::FBXImporter()
		:SpecificImporter() 
	{
		mExtensions.push_back("fbx");
	}

	FBXImporter::~FBXImporter() 
	{

	}

	bool FBXImporter::isExtensionSupported(const String& ext) const
	{
		String lowerCaseExt = ext;
		StringUtil::toLowerCase(lowerCaseExt);

		return find(mExtensions.begin(), mExtensions.end(), lowerCaseExt) != mExtensions.end();
	}

	bool FBXImporter::isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const
	{
		return true; // FBX files can be plain-text so I don't even check for magic number
	}

	ResourcePtr FBXImporter::import(const String& filePath)
	{
		FbxManager* fbxManager = nullptr;
		FbxScene* fbxScene = nullptr;

		// TODO - Set up FBX allocators

		startUpSdk(fbxManager, fbxScene);
		loadScene(fbxManager, fbxScene, filePath);

		MeshDataPtr mesh = parseScene(fbxManager, fbxScene);	

		shutDownSdk(fbxManager);

		// TODO - Free FBXMeshData

		return nullptr;
	}

	void FBXImporter::startUpSdk(FbxManager*& manager, FbxScene*& scene)
	{
		manager = FbxManager::Create();
		if(manager == nullptr)
			CM_EXCEPT(InternalErrorException, "FBX SDK failed to initialize. FbxManager::Create() failed.");

		FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
		manager->SetIOSettings(ios);

		scene = FbxScene::Create(manager, "Import Scene");
		if(scene == nullptr)
			CM_EXCEPT(InternalErrorException, "Failed to create FBX scene.");
	}

	void FBXImporter::shutDownSdk(FbxManager* manager)
	{
		manager->Destroy();
	}

	void FBXImporter::loadScene(FbxManager* manager, FbxScene* scene, const String& filePath)
	{
		int lFileMajor, lFileMinor, lFileRevision;
		int lSDKMajor,  lSDKMinor,  lSDKRevision;
		FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

		FbxImporter* importer = FbxImporter::Create(manager, "");
		bool importStatus = importer->Initialize(filePath.c_str(), -1, manager->GetIOSettings());
		
		importer->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

		if(!importStatus)
		{
			CM_EXCEPT(InternalErrorException, "Call to FbxImporter::Initialize() failed.\n" +
				String("Error returned: %s\n\n") + String(importer->GetLastErrorString()));
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

			CM_EXCEPT(InternalErrorException, "Call to FbxImporter::Initialize() failed.\n" +
				String("Error returned: %s\n\n") + String(importer->GetLastErrorString()));
		}

		importer->Destroy();
	}

	MeshDataPtr FBXImporter::parseScene(FbxManager* manager, FbxScene* scene)
	{
		stack<FbxNode*>::type todo;
		todo.push(scene->GetRootNode());

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
						if(!mesh->IsTriangleMesh())
						{
							FbxGeometryConverter geomConverter(manager);
							geomConverter.TriangulateInPlace(curNode);
							attrib = curNode->GetNodeAttribute();
							mesh = static_cast<FbxMesh*>(attrib);
						}

						// TODO - We only parse the first mesh in the file. Later when I have prefabs or something similar
						// we can parse an entire hierarchy
						return parseMesh(mesh); 
					}
					break;
				case FbxNodeAttribute::eSkeleton:
					break; // TODO - I should probably implement skeleton parsing

				}
			}

			for(int i = 0; i < curNode->GetChildCount(); i++)
				todo.push(curNode->GetChild(i));
		}

		return nullptr;
	}

	MeshDataPtr FBXImporter::parseMesh(FbxMesh* mesh, bool createTangentsIfMissing)
	{
		MeshDataPtr meshData = MeshDataPtr(new MeshData());

		if (!mesh->GetNode())
			return meshData;

		if(createTangentsIfMissing && mesh->GetElementUVCount() > 0)
			mesh->GenerateTangentsData(0, false);

		const int lPolygonCount = mesh->GetPolygonCount();

		// Count the polygon count of each material
		FbxLayerElementArrayTemplate<int>* lMaterialIndice = NULL;
		FbxGeometryElement::EMappingMode lMaterialMappingMode = FbxGeometryElement::eNone;
		if (mesh->GetElementMaterial())
		{
			lMaterialIndice = &mesh->GetElementMaterial()->GetIndexArray();
			lMaterialMappingMode = mesh->GetElementMaterial()->GetMappingMode();
			if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
			{
				FBX_ASSERT(lMaterialIndice->GetCount() == lPolygonCount);
				if (lMaterialIndice->GetCount() == lPolygonCount)
				{
					// Count the faces of each material
					for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
					{
						const int lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
						if (meshData->subMeshes.size() < lMaterialIndex + 1)
						{
							meshData->subMeshes.resize(lMaterialIndex + 1);
						}

						meshData->subMeshes[lMaterialIndex].indexCount += 3;
					}

					// Record the offset (how many vertex)
					const int lMaterialCount = meshData->subMeshes.size();
					int lOffset = 0;
					for (int lIndex = 0; lIndex < lMaterialCount; ++lIndex)
					{
						meshData->subMeshes[lIndex].indexOffset = lOffset;
						lOffset += meshData->subMeshes[lIndex].indexCount;
						// This will be used as counter in the following procedures, reset to zero
						meshData->subMeshes[lIndex].indexCount = 0;
					}
					FBX_ASSERT(lOffset == lPolygonCount * 3);
				}
			}
		}

		// All faces will use the same material.
		if (meshData->subMeshes.size() == 0)
			meshData->subMeshes.resize(1);

		// Find out which vertex attributes exist
		bool allByControlPoint = true;

		bool hasNormal = mesh->GetElementNormalCount() > 0;
		FbxGeometryElement::EMappingMode lNormalMappingMode = FbxGeometryElement::eNone;
		
		if (hasNormal)
		{
			lNormalMappingMode = mesh->GetElementNormal(0)->GetMappingMode();
			if (lNormalMappingMode == FbxGeometryElement::eNone)
			{
				hasNormal = false;
			}
			if (hasNormal && lNormalMappingMode != FbxGeometryElement::eByControlPoint)
			{
				allByControlPoint = false;
			}
		}

		bool hasTangent = mesh->GetElementTangentCount() > 0;
		FbxGeometryElement::EMappingMode lTangentMappingMode = FbxGeometryElement::eNone;

		if (hasTangent)
		{
			lTangentMappingMode = mesh->GetElementTangent(0)->GetMappingMode();
			if (lTangentMappingMode == FbxGeometryElement::eNone)
			{
				hasTangent = false;
			}
			if (hasTangent && lTangentMappingMode != FbxGeometryElement::eByControlPoint)
			{
				allByControlPoint = false;
			}
		}

		bool hasBitangent = mesh->GetElementBinormalCount() > 0;
		FbxGeometryElement::EMappingMode lBitangentMappingMode = FbxGeometryElement::eNone;

		if (hasBitangent)
		{
			lBitangentMappingMode = mesh->GetElementBinormal(0)->GetMappingMode();
			if (lBitangentMappingMode == FbxGeometryElement::eNone)
			{
				hasBitangent = false;
			}
			if (hasBitangent && lBitangentMappingMode != FbxGeometryElement::eByControlPoint)
			{
				allByControlPoint = false;
			}
		}

		bool hasUV0 = mesh->GetElementUVCount() > 0;
		FbxGeometryElement::EMappingMode lUVMappingMode0 = FbxGeometryElement::eNone;
		if (hasUV0)
		{
			lUVMappingMode0 = mesh->GetElementUV(0)->GetMappingMode();
			if (lUVMappingMode0 == FbxGeometryElement::eNone)
				hasUV0 = false;

			if (hasUV0 && lUVMappingMode0 != FbxGeometryElement::eByControlPoint)
				allByControlPoint = false;
		}

		bool hasUV1 = mesh->GetElementUVCount() > 1;
		FbxGeometryElement::EMappingMode lUVMappingMode1 = FbxGeometryElement::eNone;
		if (hasUV1)
		{
			lUVMappingMode1 = mesh->GetElementUV(1)->GetMappingMode();
			if (lUVMappingMode1 == FbxGeometryElement::eNone)
				hasUV1 = false;

			if (hasUV1 && lUVMappingMode1 != FbxGeometryElement::eByControlPoint)
				allByControlPoint = false;
		}

		// Allocate the array memory, by control point or by polygon vertex.
		int lPolygonVertexCount = mesh->GetControlPointsCount();
		if (!allByControlPoint)
			lPolygonVertexCount = lPolygonCount * 3;

		meshData->indexCount = lPolygonCount * 3;
		meshData->vertexCount = lPolygonVertexCount;
		
		meshData->index = new int[lPolygonCount * 3];

		std::shared_ptr<MeshData::VertexData> vertexData = std::shared_ptr<MeshData::VertexData>(new MeshData::VertexData(lPolygonVertexCount));
		meshData->vertexBuffers.insert(std::make_pair(0, vertexData));
		vertexData->vertex = new Vector3[lPolygonVertexCount];

		if (hasNormal)
			vertexData->normal = new Vector3[lPolygonVertexCount];

		if (hasTangent)
			vertexData->tangent = new Vector3[lPolygonVertexCount];

		if (hasBitangent)
			vertexData->bitangent = new Vector3[lPolygonVertexCount];

		FbxStringList lUVNames;
		mesh->GetUVSetNames(lUVNames);
		const char * lUVName0 = NULL;
		if (hasUV0 && lUVNames.GetCount() > 0)
		{
			vertexData->uv0 = new Vector2[lPolygonVertexCount];
			lUVName0 = lUVNames[0];
		}

		const char * lUVName1 = NULL;
		if (hasUV1 && lUVNames.GetCount() > 1)
		{
			vertexData->uv1 = new Vector2[lPolygonVertexCount];
			lUVName1 = lUVNames[1];
		}

		// Populate the array with vertex attribute, if by control point.
		const FbxVector4 * lControlPoints = mesh->GetControlPoints();
		FbxVector4 lCurrentVertex;
		FbxVector4 lCurrentNormal;
		FbxVector2 lCurrentUV;

		const FbxGeometryElementTangent * lTangentElement = NULL;
		if (hasTangent)
			lTangentElement = mesh->GetElementTangent(0);

		const FbxGeometryElementBinormal * lBitangentElement = NULL;
		if (hasBitangent)
			lBitangentElement = mesh->GetElementBinormal(0);

		if (allByControlPoint)
		{
			const FbxGeometryElementNormal * lNormalElement = NULL;
			if (hasNormal)
				lNormalElement = mesh->GetElementNormal(0);

			const FbxGeometryElementUV * lUVElement0 = NULL;
			if (hasUV0)
				lUVElement0 = mesh->GetElementUV(0);

			const FbxGeometryElementUV * lUVElement1 = NULL;
			if (hasUV1)
				lUVElement1 = mesh->GetElementUV(1);

			for (int lIndex = 0; lIndex < lPolygonVertexCount; ++lIndex)
			{
				// Save the vertex position.
				lCurrentVertex = lControlPoints[lIndex];
				vertexData->vertex[lIndex][0] = static_cast<float>(lCurrentVertex[0]);
				vertexData->vertex[lIndex][1] = static_cast<float>(lCurrentVertex[1]);
				vertexData->vertex[lIndex][2] = static_cast<float>(lCurrentVertex[2]);

				// Save the normal.
				if (hasNormal)
				{
					int lNormalIndex = lIndex;
					if (lNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndex);

					lCurrentNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
					vertexData->normal[lIndex][0] = static_cast<float>(lCurrentNormal[0]);
					vertexData->normal[lIndex][1] = static_cast<float>(lCurrentNormal[1]);
					vertexData->normal[lIndex][2] = static_cast<float>(lCurrentNormal[2]);
				}

				// Save the tangent.
				if (hasTangent)
				{
					int lTangentIndex = lIndex;
					if (lTangentElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						lTangentIndex = lTangentElement->GetIndexArray().GetAt(lIndex);

					FbxVector4 lCurrentTangent = lTangentElement->GetDirectArray().GetAt(lTangentIndex);
					vertexData->tangent[lIndex][0] = static_cast<float>(lCurrentTangent[0]);
					vertexData->tangent[lIndex][1] = static_cast<float>(lCurrentTangent[1]);
					vertexData->tangent[lIndex][2] = static_cast<float>(lCurrentTangent[2]);
				}

				// Save the tangent.
				if (hasBitangent)
				{
					int lBitangentIndex = lIndex;
					if (lBitangentElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						lBitangentIndex = lBitangentElement->GetIndexArray().GetAt(lIndex);

					FbxVector4 lCurrentBitangent = lBitangentElement->GetDirectArray().GetAt(lBitangentIndex);
					vertexData->bitangent[lIndex][0] = static_cast<float>(lCurrentBitangent[0]);
					vertexData->bitangent[lIndex][1] = static_cast<float>(lCurrentBitangent[1]);
					vertexData->bitangent[lIndex][2] = static_cast<float>(lCurrentBitangent[2]);
				}

				// Save the UV.
				if (hasUV0)
				{
					int lUVIndex = lIndex;
					if (lUVElement0->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						lUVIndex = lUVElement0->GetIndexArray().GetAt(lIndex);

					lCurrentUV = lUVElement0->GetDirectArray().GetAt(lUVIndex);
					vertexData->uv0[lIndex][0] = static_cast<float>(lCurrentUV[0]);
					vertexData->uv0[lIndex][1] = static_cast<float>(lCurrentUV[1]);
				}

				if (hasUV1)
				{
					int lUVIndex = lIndex;
					if (lUVElement1->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						lUVIndex = lUVElement1->GetIndexArray().GetAt(lIndex);

					lCurrentUV = lUVElement1->GetDirectArray().GetAt(lUVIndex);
					vertexData->uv1[lIndex][0] = static_cast<float>(lCurrentUV[0]);
					vertexData->uv1[lIndex][1] = static_cast<float>(lCurrentUV[1]);
				}
			}
		}

		int lVertexCount = 0;
		for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
		{
			// The material for current face.
			int lMaterialIndex = 0;
			if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
			{
				lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
			}

			// Where should I save the vertex attribute index, according to the material
			const int lIndexOffset = meshData->subMeshes[lMaterialIndex].indexOffset + meshData->subMeshes[lMaterialIndex].indexCount;
			for (int lVerticeIndex = 0; lVerticeIndex < 3; ++lVerticeIndex)
			{
				const int lControlPointIndex = mesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);

				if (allByControlPoint)
				{
					meshData->index[lIndexOffset + lVerticeIndex] = static_cast<unsigned int>(lControlPointIndex);
				}
				// Populate the array with vertex attribute, if by polygon vertex.
				else
				{
					meshData->index[lIndexOffset + lVerticeIndex] = static_cast<unsigned int>(lVertexCount);

					lCurrentVertex = lControlPoints[lControlPointIndex];
					vertexData->vertex[lVertexCount][0] = static_cast<float>(lCurrentVertex[0]);
					vertexData->vertex[lVertexCount][1] = static_cast<float>(lCurrentVertex[1]);
					vertexData->vertex[lVertexCount][2] = static_cast<float>(lCurrentVertex[2]);

					if (hasNormal)
					{
						mesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, lCurrentNormal);
						vertexData->normal[lVertexCount][0] = static_cast<float>(lCurrentNormal[0]);
						vertexData->normal[lVertexCount][1] = static_cast<float>(lCurrentNormal[1]);
						vertexData->normal[lVertexCount][2] = static_cast<float>(lCurrentNormal[2]);
					}

					if (hasTangent)
					{
						int lTangentIndex = lIndexOffset + lVerticeIndex; // TODO - Is this right?
						if (lTangentElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
							lTangentIndex = lTangentElement->GetIndexArray().GetAt(lTangentIndex);

						FbxVector4 lCurrentTangent = lTangentElement->GetDirectArray().GetAt(lTangentIndex);
						vertexData->tangent[lVertexCount][0] = static_cast<float>(lCurrentTangent[0]);
						vertexData->tangent[lVertexCount][1] = static_cast<float>(lCurrentTangent[1]);
						vertexData->tangent[lVertexCount][2] = static_cast<float>(lCurrentTangent[2]);
					}

					if (hasBitangent)
					{
						int lBitangentIndex = lIndexOffset + lVerticeIndex; // TODO - Is this right?
						if (lBitangentElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
							lBitangentIndex = lBitangentElement->GetIndexArray().GetAt(lBitangentIndex);

						FbxVector4 lCurrentBitangent = lBitangentElement->GetDirectArray().GetAt(lBitangentIndex);
						vertexData->bitangent[lVertexCount][0] = static_cast<float>(lCurrentBitangent[0]);
						vertexData->bitangent[lVertexCount][1] = static_cast<float>(lCurrentBitangent[1]);
						vertexData->bitangent[lVertexCount][2] = static_cast<float>(lCurrentBitangent[2]);
					}

					if (hasNormal)
					{
						mesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, lCurrentNormal);
						vertexData->normal[lVertexCount][0] = static_cast<float>(lCurrentNormal[0]);
						vertexData->normal[lVertexCount][1] = static_cast<float>(lCurrentNormal[1]);
						vertexData->normal[lVertexCount][2] = static_cast<float>(lCurrentNormal[2]);
					}

					if (hasUV0)
					{
						mesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUVName0, lCurrentUV);
						vertexData->uv0[lVertexCount][0] = static_cast<float>(lCurrentUV[0]);
						vertexData->uv0[lVertexCount][1] = static_cast<float>(lCurrentUV[1]);
					}

					if (hasUV1)
					{
						mesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUVName1, lCurrentUV);
						vertexData->uv1[lVertexCount][0] = static_cast<float>(lCurrentUV[0]);
						vertexData->uv1[lVertexCount][1] = static_cast<float>(lCurrentUV[1]);
					}
				}
				++lVertexCount;
			}

			meshData->subMeshes[lMaterialIndex].indexCount += 3;
		}

	}
}