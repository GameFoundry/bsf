#include "CmFBXImporter.h"
#include "CmResource.h"
#include "CmDebug.h"
#include "CmDataStream.h"
#include "CmPath.h"
#include "CmMeshData.h"
#include "CmMesh.h"
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

	HResource FBXImporter::import(const String& filePath, ConstImportOptionsPtr importOptions)
	{
		FbxManager* fbxManager = nullptr;
		FbxScene* fbxScene = nullptr;

		startUpSdk(fbxManager, fbxScene);
		loadScene(fbxManager, fbxScene, filePath);

		MeshDataPtr meshData = parseScene(fbxManager, fbxScene);	

		shutDownSdk(fbxManager);

		HMesh mesh = Mesh::create();

		mesh.waitUntilLoaded();
		mesh->setMeshData(meshData);

		return mesh;
	}

	void FBXImporter::startUpSdk(FbxManager*& manager, FbxScene*& scene)
	{
		// TODO Low priority - Initialize allocator methods for FBX. It calls a lot of heap allocs (200 000 calls for a simple 2k poly mesh) which slows down the import.
		// Custom allocator would help a lot.

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

		vector<MeshDataPtr>::type allMeshes;

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

						MeshDataPtr meshData = parseMesh(mesh); 
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
			return allMeshes[0];
		else
			return MeshData::combine(allMeshes);
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
		vector<SubMesh>::type subMeshes;
		vector<UINT32*>::type indices;
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
						const UINT32 lMaterialIndex = (UINT32)lMaterialIndice->GetAt(lPolygonIndex);
						if (subMeshes.size() < lMaterialIndex + 1)
						{
							subMeshes.resize(lMaterialIndex + 1);
						}

						subMeshes[lMaterialIndex].indexCount += 3;
					}

					// Record the offsets and allocate index arrays
					indices.resize(subMeshes.size());
					const int lMaterialCount = (const int)subMeshes.size();
					int lOffset = 0;
					for (int lIndex = 0; lIndex < lMaterialCount; ++lIndex)
					{
						subMeshes[lIndex].indexOffset = lOffset;
						lOffset += subMeshes[lIndex].indexCount;
						indices[lIndex] = meshData->addIndices32(subMeshes[lIndex].indexCount, lIndex);
					}
					FBX_ASSERT(lOffset == lPolygonCount * 3);
				}
			}
		}

		// All faces will use the same material.
		if (subMeshes.size() == 0)
		{
			subMeshes.resize(1);
			indices.resize(1);
			subMeshes[0].indexCount = lPolygonCount * 3;
			indices[0] = meshData->addIndices32(subMeshes[0].indexCount);
		}

		// Find out which vertex attributes exist
		bool allByControlPoint = true;

		bool hasColor = mesh->GetElementVertexColorCount() > 0;
		FbxGeometryElement::EMappingMode lColorMappingMode = FbxGeometryElement::eNone;

		if(hasColor)
		{
			lColorMappingMode = mesh->GetElementVertexColor(0)->GetMappingMode();
			if (lColorMappingMode == FbxGeometryElement::eNone)
			{
				hasColor = false;
			}
			if (hasColor && lColorMappingMode != FbxGeometryElement::eByControlPoint)
			{
				allByControlPoint = false;
			}
		}

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

		UINT32 vertexCount = lPolygonVertexCount;
		Vector3* vertex = meshData->addPositionsVec3(vertexCount);

		Color* color = nullptr;
		if(hasColor)
			color = meshData->addColorsFloat(vertexCount);

		Vector3* normal = nullptr;
		if (hasNormal)
			normal = meshData->addNormals(vertexCount);

		Vector3* tangent = nullptr;
		if (hasTangent)
			tangent = meshData->addTangentsVec3(vertexCount);

		Vector3* bitangent = nullptr;
		if (hasBitangent)
			bitangent = meshData->addBitangents(vertexCount);

		FbxStringList lUVNames;
		mesh->GetUVSetNames(lUVNames);
		const char * lUVName0 = NULL;
		Vector2* uv0 = nullptr;
		if (hasUV0 && lUVNames.GetCount() > 0)
		{
			uv0 = meshData->addUV0(vertexCount);
			lUVName0 = lUVNames[0];
		}

		const char * lUVName1 = NULL;
		Vector2* uv1 = nullptr;
		if (hasUV1 && lUVNames.GetCount() > 1)
		{
			uv1 = meshData->addUV1(vertexCount);
			lUVName1 = lUVNames[1];
		}

		// Populate the array with vertex attribute, if by control point.
		const FbxVector4 * lControlPoints = mesh->GetControlPoints();
		FbxVector4 lCurrentVertex;
		FbxVector4 lCurrentNormal;
		FbxVector2 lCurrentUV;

		const FbxGeometryElementVertexColor * lColorElement = NULL;
		if (hasColor)
			lColorElement = mesh->GetElementVertexColor(0);

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
				vertex[lIndex][0] = static_cast<float>(lCurrentVertex[0]);
				vertex[lIndex][1] = static_cast<float>(lCurrentVertex[1]);
				vertex[lIndex][2] = static_cast<float>(lCurrentVertex[2]);

				// Save vertex color
				if(hasColor)
				{
					int lColorIndex = lIndex;
					if (lColorElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						lColorIndex = lColorElement->GetIndexArray().GetAt(lIndex);

					FbxColor lCurrentColor = lColorElement->GetDirectArray().GetAt(lColorIndex);
					color[lIndex][0] = static_cast<float>(lCurrentColor[0]);
					color[lIndex][1] = static_cast<float>(lCurrentColor[1]);
					color[lIndex][2] = static_cast<float>(lCurrentColor[2]);
					color[lIndex][3] = static_cast<float>(lCurrentColor[3]);
				}

				// Save the normal.
				if (hasNormal)
				{
					int lNormalIndex = lIndex;
					if (lNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndex);

					lCurrentNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
					normal[lIndex][0] = static_cast<float>(lCurrentNormal[0]);
					normal[lIndex][1] = static_cast<float>(lCurrentNormal[1]);
					normal[lIndex][2] = static_cast<float>(lCurrentNormal[2]);
				}

				// Save the tangent.
				if (hasTangent)
				{
					int lTangentIndex = lIndex;
					if (lTangentElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						lTangentIndex = lTangentElement->GetIndexArray().GetAt(lIndex);

					FbxVector4 lCurrentTangent = lTangentElement->GetDirectArray().GetAt(lTangentIndex);
					tangent[lIndex][0] = static_cast<float>(lCurrentTangent[0]);
					tangent[lIndex][1] = static_cast<float>(lCurrentTangent[1]);
					tangent[lIndex][2] = static_cast<float>(lCurrentTangent[2]);
				}

				// Save the bitangent.
				if (hasBitangent)
				{
					int lBitangentIndex = lIndex;
					if (lBitangentElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						lBitangentIndex = lBitangentElement->GetIndexArray().GetAt(lIndex);

					FbxVector4 lCurrentBitangent = lBitangentElement->GetDirectArray().GetAt(lBitangentIndex);
					bitangent[lIndex][0] = static_cast<float>(lCurrentBitangent[0]);
					bitangent[lIndex][1] = static_cast<float>(lCurrentBitangent[1]);
					bitangent[lIndex][2] = static_cast<float>(lCurrentBitangent[2]);
				}

				// Save the UV.
				if (hasUV0)
				{
					int lUVIndex = lIndex;
					if (lUVElement0->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						lUVIndex = lUVElement0->GetIndexArray().GetAt(lIndex);

					lCurrentUV = lUVElement0->GetDirectArray().GetAt(lUVIndex);
					uv0[lIndex][0] = static_cast<float>(lCurrentUV[0]);
					uv0[lIndex][1] = static_cast<float>(lCurrentUV[1]);
				}

				if (hasUV1)
				{
					int lUVIndex = lIndex;
					if (lUVElement1->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						lUVIndex = lUVElement1->GetIndexArray().GetAt(lIndex);

					lCurrentUV = lUVElement1->GetDirectArray().GetAt(lUVIndex);
					uv1[lIndex][0] = static_cast<float>(lCurrentUV[0]);
					uv1[lIndex][1] = static_cast<float>(lCurrentUV[1]);
				}
			}
		}

		int lVertexCount = 0;
		vector<UINT32>::type indexOffsetPerSubmesh;
		indexOffsetPerSubmesh.resize(subMeshes.size(), 0);

		for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; ++lPolygonIndex)
		{
			// The material for current face.
			int lMaterialIndex = 0;
			if (lMaterialIndice && lMaterialMappingMode == FbxGeometryElement::eByPolygon)
			{
				lMaterialIndex = lMaterialIndice->GetAt(lPolygonIndex);
			}

			// Where should I save the vertex attribute index, according to the material
			int lIndexOffset = subMeshes[lMaterialIndex].indexOffset + indexOffsetPerSubmesh[lMaterialIndex];
			for (int lVerticeIndex = 0; lVerticeIndex < 3; ++lVerticeIndex)
			{
				const int lControlPointIndex = mesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);

				if (allByControlPoint)
				{
					indices[lMaterialIndex][lIndexOffset + lVerticeIndex] = static_cast<unsigned int>(lControlPointIndex);
				}
				// Populate the array with vertex attribute, if by polygon vertex.
				else
				{
					indices[lMaterialIndex][lIndexOffset + lVerticeIndex] = static_cast<unsigned int>(lVertexCount);

					lCurrentVertex = lControlPoints[lControlPointIndex];
					vertex[lVertexCount][0] = static_cast<float>(lCurrentVertex[0]);
					vertex[lVertexCount][1] = static_cast<float>(lCurrentVertex[1]);
					vertex[lVertexCount][2] = static_cast<float>(lCurrentVertex[2]);

					if(hasColor)
					{
						int lColorIndex = lIndexOffset + lVerticeIndex; // TODO - Is this right?
						if (lColorElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
							lColorIndex = lColorElement->GetIndexArray().GetAt(lColorIndex);

						FbxColor lCurrentColor = lColorElement->GetDirectArray().GetAt(lColorIndex);
						color[lVertexCount][0] = static_cast<float>(lCurrentColor[0]);
						color[lVertexCount][1] = static_cast<float>(lCurrentColor[1]);
						color[lVertexCount][2] = static_cast<float>(lCurrentColor[2]);
						color[lVertexCount][3] = static_cast<float>(lCurrentColor[3]);
					}

					if (hasNormal)
					{
						mesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, lCurrentNormal);
						normal[lVertexCount][0] = static_cast<float>(lCurrentNormal[0]);
						normal[lVertexCount][1] = static_cast<float>(lCurrentNormal[1]);
						normal[lVertexCount][2] = static_cast<float>(lCurrentNormal[2]);
					}

					if (hasTangent)
					{
						int lTangentIndex = lIndexOffset + lVerticeIndex; // TODO - Is this right?
						if (lTangentElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
							lTangentIndex = lTangentElement->GetIndexArray().GetAt(lTangentIndex);

						FbxVector4 lCurrentTangent = lTangentElement->GetDirectArray().GetAt(lTangentIndex);
						tangent[lVertexCount][0] = static_cast<float>(lCurrentTangent[0]);
						tangent[lVertexCount][1] = static_cast<float>(lCurrentTangent[1]);
						tangent[lVertexCount][2] = static_cast<float>(lCurrentTangent[2]);
					}

					if (hasBitangent)
					{
						int lBitangentIndex = lIndexOffset + lVerticeIndex; // TODO - Is this right?
						if (lBitangentElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
							lBitangentIndex = lBitangentElement->GetIndexArray().GetAt(lBitangentIndex);

						FbxVector4 lCurrentBitangent = lBitangentElement->GetDirectArray().GetAt(lBitangentIndex);
						bitangent[lVertexCount][0] = static_cast<float>(lCurrentBitangent[0]);
						bitangent[lVertexCount][1] = static_cast<float>(lCurrentBitangent[1]);
						bitangent[lVertexCount][2] = static_cast<float>(lCurrentBitangent[2]);
					}

					if (hasUV0)
					{
						mesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUVName0, lCurrentUV);
						uv0[lVertexCount][0] = static_cast<float>(lCurrentUV[0]);
						uv0[lVertexCount][1] = static_cast<float>(lCurrentUV[1]);
					}

					if (hasUV1)
					{
						mesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUVName1, lCurrentUV);
						uv1[lVertexCount][0] = static_cast<float>(lCurrentUV[0]);
						uv1[lVertexCount][1] = static_cast<float>(lCurrentUV[1]);
					}
				}
				++lVertexCount;
			}

			indexOffsetPerSubmesh[lMaterialIndex] += 3;
		}

		return meshData;
	}
}