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

	BaseResourceHandle FBXImporter::import(const String& filePath)
	{
		FbxManager* fbxManager = nullptr;
		FbxScene* fbxScene = nullptr;

		startUpSdk(fbxManager, fbxScene);
		loadScene(fbxManager, fbxScene, filePath);

		MeshDataPtr meshData = parseScene(fbxManager, fbxScene);	

		shutDownSdk(fbxManager);

		MeshHandle mesh(new Mesh());
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
			return mergeMeshData(allMeshes);
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
						const UINT32 lMaterialIndex = (UINT32)lMaterialIndice->GetAt(lPolygonIndex);
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

		meshData->indexCount = lPolygonCount * 3;
		meshData->vertexCount = lPolygonVertexCount;
		
		meshData->index = new int[lPolygonCount * 3];

		std::shared_ptr<MeshData::VertexData> vertexData = std::shared_ptr<MeshData::VertexData>(new MeshData::VertexData(lPolygonVertexCount));
		meshData->vertexBuffers.insert(std::make_pair(0, vertexData));
		vertexData->vertex = new Vector3[lPolygonVertexCount];

		if(hasColor)
			vertexData->color = new Color[lPolygonVertexCount];

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
				vertexData->vertex[lIndex][0] = static_cast<float>(lCurrentVertex[0]);
				vertexData->vertex[lIndex][1] = static_cast<float>(lCurrentVertex[1]);
				vertexData->vertex[lIndex][2] = static_cast<float>(lCurrentVertex[2]);

				// Save vertex color
				if(hasColor)
				{
					int lColorIndex = lIndex;
					if (lColorElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						lColorIndex = lColorElement->GetIndexArray().GetAt(lIndex);

					FbxColor lCurrentColor = lColorElement->GetDirectArray().GetAt(lColorIndex);
					vertexData->color[lIndex][0] = static_cast<float>(lCurrentColor[0]);
					vertexData->color[lIndex][1] = static_cast<float>(lCurrentColor[1]);
					vertexData->color[lIndex][2] = static_cast<float>(lCurrentColor[2]);
					vertexData->color[lIndex][3] = static_cast<float>(lCurrentColor[3]);
				}

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

					if(hasColor)
					{
						int lColorIndex = lIndexOffset + lVerticeIndex; // TODO - Is this right?
						if (lColorElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
							lColorIndex = lColorElement->GetIndexArray().GetAt(lColorIndex);

						FbxColor lCurrentColor = lColorElement->GetDirectArray().GetAt(lColorIndex);
						vertexData->color[lVertexCount][0] = static_cast<float>(lCurrentColor[0]);
						vertexData->color[lVertexCount][1] = static_cast<float>(lCurrentColor[1]);
						vertexData->color[lVertexCount][2] = static_cast<float>(lCurrentColor[2]);
						vertexData->color[lVertexCount][3] = static_cast<float>(lCurrentColor[3]);
					}

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

		initDeclarationForMeshData(meshData);

		return meshData;
	}

	void FBXImporter::initDeclarationForMeshData(MeshDataPtr meshData)
	{
		std::shared_ptr<MeshData::VertexData> vertexData = meshData->vertexBuffers[0];

		UINT32 offset = 0;
		if(vertexData->vertex)
		{
			meshData->declaration->addElement(0, offset, VET_FLOAT3, VES_POSITION, 0);
			offset += VertexElement::getTypeSize(VET_FLOAT3);
		}

		if(vertexData->color)
		{
			meshData->declaration->addElement(0, offset, VET_COLOUR, VES_DIFFUSE, 0);
			offset += VertexElement::getTypeSize(VET_COLOUR);
		}

		if(vertexData->normal)
		{
			meshData->declaration->addElement(0, offset, VET_FLOAT3, VES_NORMAL, 0);
			offset += VertexElement::getTypeSize(VET_FLOAT3);
		}

		if(vertexData->tangent)
		{
			meshData->declaration->addElement(0, offset, VET_FLOAT3, VES_TANGENT, 0);
			offset += VertexElement::getTypeSize(VET_FLOAT3);
		}

		// TODO - Storing bitangents with the mesh is probably not a good idea. It's likely cheaper to recreate them in the shader
		if(vertexData->bitangent)
		{
			meshData->declaration->addElement(0, offset, VET_FLOAT3, VES_BITANGENT, 0);
			offset += VertexElement::getTypeSize(VET_FLOAT3);
		}

		if(vertexData->uv0)
		{
			meshData->declaration->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
			offset += VertexElement::getTypeSize(VET_FLOAT2);
		}

		if(vertexData->uv1)
		{
			meshData->declaration->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 1);
			offset += VertexElement::getTypeSize(VET_FLOAT2);
		}
	}

	MeshDataPtr FBXImporter::mergeMeshData(vector<MeshDataPtr>::type meshes)
	{
		// TODO Low priority - Throughout this method we're assuming mesh data only has a single stream, which is a fair assumption now, but that might change later.

		MeshDataPtr meshData = MeshDataPtr(new MeshData());

		bool hasPosition = false;
		bool hasColors = false;
		bool hasNormals = false;
		bool hasTangents = false;
		bool hasBitangents = false;
		bool hasUV0 = false;
		bool hasUV1 = false;

		// Count all vertices and indexes. And determine all data types.
		for(auto iter = meshes.begin(); iter != meshes.end(); ++iter)
		{
			meshData->indexCount += (*iter)->indexCount;
			meshData->vertexCount += (*iter)->vertexCount;

			std::shared_ptr<MeshData::VertexData> vertData = (*iter)->vertexBuffers[0];
			if(vertData)
			{
				if(vertData->vertex)
					hasPosition = true;

				if(vertData->color)
					hasColors = true;

				if(vertData->normal)
					hasNormals = true;

				if(vertData->tangent)
					hasTangents = true;

				if(vertData->bitangent)
					hasBitangents = true;

				if(vertData->uv0)
					hasUV0 = true;

				if(vertData->uv1)
					hasUV1 = true;
			}
		}

		// Copy indices
		meshData->index = new int[meshData->indexCount];

		int currentIndexIdx = 0;
		int currentVertIdx = 0;
		for(auto iter = meshes.begin(); iter != meshes.end(); ++iter)
		{
			int indexCount = (*iter)->indexCount;

			for(int i = 0; i < indexCount; i++)
				meshData->index[currentIndexIdx + i] = (*iter)->index[i] + currentVertIdx;

			currentIndexIdx += indexCount;
			currentVertIdx += (*iter)->vertexCount;
		}

		// Copy vertex data
		std::shared_ptr<MeshData::VertexData> combinedVertData(new MeshData::VertexData(meshData->vertexCount, 0));
		meshData->vertexBuffers[0] = combinedVertData;

		if(hasPosition)
			combinedVertData->vertex = new Vector3[meshData->vertexCount];

		if(hasColors)
			combinedVertData->color = new Color[meshData->vertexCount];

		if(hasNormals)
			combinedVertData->normal = new Vector3[meshData->vertexCount];

		if(hasTangents)
			combinedVertData->tangent = new Vector3[meshData->vertexCount];

		if(hasBitangents)
			combinedVertData->bitangent = new Vector3[meshData->vertexCount];

		if(hasUV0)
			combinedVertData->uv0 = new Vector2[meshData->vertexCount];

		if(hasUV1)
			combinedVertData->uv1 = new Vector2[meshData->vertexCount];

		currentVertIdx = 0;
		for(auto iter = meshes.begin(); iter != meshes.end(); ++iter)
		{
			int indexCount = (*iter)->indexCount;

			std::shared_ptr<MeshData::VertexData> vertData = (*iter)->vertexBuffers[0];

			if(hasPosition)
			{
				if(vertData && vertData->vertex)
					memcpy(&combinedVertData->vertex[currentVertIdx], vertData->vertex, (*iter)->vertexCount * sizeof(Vector3));
				else
					memset(&combinedVertData->vertex[currentVertIdx], 0, (*iter)->vertexCount * sizeof(Vector3));
			}

			if(hasColors)
			{
				if(vertData && vertData->color)
					memcpy(&combinedVertData->color[currentVertIdx], vertData->color, (*iter)->vertexCount * sizeof(Color));
				else
					memset(&combinedVertData->color[currentVertIdx], 0, (*iter)->vertexCount * sizeof(Color));
			}

			if(hasNormals)
			{
				if(vertData && vertData->normal)
					memcpy(&combinedVertData->normal[currentVertIdx], vertData->normal, (*iter)->vertexCount * sizeof(Vector3));
				else
					memset(&combinedVertData->normal[currentVertIdx], 0, (*iter)->vertexCount * sizeof(Vector3));
			}

			if(hasTangents)
			{
				if(vertData && vertData->tangent)
					memcpy(&combinedVertData->tangent[currentVertIdx], vertData->tangent, (*iter)->vertexCount * sizeof(Vector3));
				else
					memset(&combinedVertData->tangent[currentVertIdx], 0, (*iter)->vertexCount * sizeof(Vector3));
			}

			if(hasBitangents)
			{
				if(vertData && vertData->bitangent)
					memcpy(&combinedVertData->bitangent[currentVertIdx], vertData->bitangent, (*iter)->vertexCount * sizeof(Vector3));
				else
					memset(&combinedVertData->bitangent[currentVertIdx], 0, (*iter)->vertexCount * sizeof(Vector3));
			}

			if(hasUV0)
			{
				if(vertData && vertData->uv0)
					memcpy(&combinedVertData->uv0[currentVertIdx], vertData->uv0, (*iter)->vertexCount * sizeof(Vector2));
				else
					memset(&combinedVertData->uv0[currentVertIdx], 0, (*iter)->vertexCount * sizeof(Vector2));
			}

			if(hasUV1)
			{
				if(vertData && vertData->uv1)
					memcpy(&combinedVertData->uv1[currentVertIdx], vertData->uv1, (*iter)->vertexCount * sizeof(Vector2));
				else
					memset(&combinedVertData->uv1[currentVertIdx], 0, (*iter)->vertexCount * sizeof(Vector2));
			}

			currentVertIdx += (*iter)->vertexCount;
		}

		// Copy submesh data
		currentIndexIdx = 0;
		for(auto iter = meshes.begin(); iter != meshes.end(); ++iter)
		{
			UINT32 subMeshCount = (*iter)->subMeshes.size();

			for(UINT32 i = 0; i < subMeshCount; i++)
			{
				MeshData::SubMeshData newSubMesh;
				newSubMesh.indexCount = (*iter)->subMeshes[i].indexCount;
				newSubMesh.indexOffset = (*iter)->subMeshes[i].indexOffset + currentIndexIdx;

				meshData->subMeshes.push_back(newSubMesh);
			}

			currentIndexIdx += (*iter)->indexCount;
		}

		initDeclarationForMeshData(meshData);

		return meshData;
	}
}