#include "CmFBXImporter.h"
#include "CmResource.h"
#include "CmApplication.h"
#include "CmDebug.h"
#include "CmDataStream.h"
#include "CmPath.h"
#include "CmMeshData.h"
#include "CmMesh.h"
#include "CmVector2.h"
#include "CmVector3.h"
#include "CmVector4.h"
#include "CmVertexDataDesc.h"

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

	ResourcePtr FBXImporter::import(const WString& filePath, ConstImportOptionsPtr importOptions)
	{
		FbxManager* fbxManager = nullptr;
		FbxScene* fbxScene = nullptr;

		startUpSdk(fbxManager, fbxScene);
		loadScene(fbxManager, fbxScene, filePath);

		Vector<SubMesh>::type subMeshes;
		MeshDataPtr meshData = parseScene(fbxManager, fbxScene, subMeshes);	

		shutDownSdk(fbxManager);

		MeshPtr mesh = Mesh::_createPtr(meshData);
		mesh->setSubMeshes(subMeshes);

		WString fileName = Path::getFilename(filePath, false);
		mesh->setName(toString(fileName));

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

	void FBXImporter::loadScene(FbxManager* manager, FbxScene* scene, const WString& filePath)
	{
		int lFileMajor, lFileMinor, lFileRevision;
		int lSDKMajor,  lSDKMinor,  lSDKRevision;
		FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

		FbxImporter* importer = FbxImporter::Create(manager, "");
		bool importStatus = importer->Initialize(toString(filePath).c_str(), -1, manager->GetIOSettings());
		
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

	MeshDataPtr FBXImporter::parseScene(FbxManager* manager, FbxScene* scene, Vector<SubMesh>::type& subMeshes)
	{
		Stack<FbxNode*>::type todo;
		todo.push(scene->GetRootNode());

		Vector<MeshDataPtr>::type allMeshes;
		Vector<Vector<SubMesh>::type>::type allSubMeshes;

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

						allSubMeshes.push_back(Vector<SubMesh>::type());

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
			return MeshData::combine(allMeshes, allSubMeshes, subMeshes);
	}

	MeshDataPtr FBXImporter::parseMesh(FbxMesh* mesh, Vector<SubMesh>::type& subMeshes, bool createTangentsIfMissing)
	{
		if (!mesh->GetNode())
		{
			VertexDataDescPtr tmpVertDesc = cm_shared_ptr<VertexDataDesc>();
			return cm_shared_ptr<MeshData, ScratchAlloc>(0, 0, tmpVertDesc);
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

		// Create tangents if needed
		if(createTangentsIfMissing && mesh->GetElementUVCount() > 0)
			mesh->GenerateTangentsData(0, false);

		// Calculate number of vertices and indexes
		const int lPolygonCount = mesh->GetPolygonCount();

		int lPolygonVertexCount = mesh->GetControlPointsCount();
		if (!allByControlPoint)
			lPolygonVertexCount = lPolygonCount * 3;

		UINT32 vertexCount = lPolygonVertexCount;

		VertexDataDescPtr vertexDesc = cm_shared_ptr<VertexDataDesc>();

		vertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);

		if(hasColor)
			vertexDesc->addVertElem(VET_COLOR, VES_COLOR);

		if(hasNormal)
			vertexDesc->addVertElem(VET_FLOAT3, VES_NORMAL);

		if(hasTangent)
			vertexDesc->addVertElem(VET_FLOAT3, VES_TANGENT);

		if(hasBitangent)
			vertexDesc->addVertElem(VET_FLOAT3, VES_BITANGENT);

		FbxStringList lUVNames;
		mesh->GetUVSetNames(lUVNames);
		const char * lUVName0 = NULL;
		if (hasUV0 && lUVNames.GetCount() > 0)
		{
			vertexDesc->addVertElem(VET_FLOAT2, VES_TEXCOORD, 0);
			lUVName0 = lUVNames[0];
		}

		const char * lUVName1 = NULL;
		if (hasUV1 && lUVNames.GetCount() > 1)
		{
			vertexDesc->addVertElem(VET_FLOAT2, VES_TEXCOORD, 1);
			lUVName1 = lUVNames[1];
		}

		// Count the polygon count of each material
		FbxLayerElementArrayTemplate<int>* lMaterialIndice = NULL;
		FbxGeometryElement::EMappingMode lMaterialMappingMode = FbxGeometryElement::eNone;

		UINT32 numIndices = 0;
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
					const int lMaterialCount = (const int)subMeshes.size();
					int lOffset = 0;
					for (int lIndex = 0; lIndex < lMaterialCount; ++lIndex)
					{
						subMeshes[lIndex].indexOffset = lOffset;
						lOffset += subMeshes[lIndex].indexCount;

						numIndices += subMeshes[lIndex].indexCount;
					}
					FBX_ASSERT(lOffset == lPolygonCount * 3);
				}
			}
		}

		// All faces will use the same material.
		if (subMeshes.size() == 0)
		{
			numIndices = lPolygonCount * 3;

			subMeshes.resize(1);
			subMeshes[0].indexCount = lPolygonCount * 3;
		}

		MeshDataPtr meshData = cm_shared_ptr<MeshData, ScratchAlloc>(vertexCount, numIndices, vertexDesc);

		// Allocate the array memory, by control point or by polygon vertex.
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
		if (hasUV0 && lUVNames.GetCount() > 0)
		{
			uv0 = meshData->getVec2DataIter(VES_TEXCOORD, 0);
			lUVName0 = lUVNames[0];
		}

		VertexElemIter<Vector2> uv1;
		if (hasUV1 && lUVNames.GetCount() > 1)
		{
			uv1 = meshData->getVec2DataIter(VES_TEXCOORD, 1);
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

				Vector3 curPosValue;
				curPosValue[0] = static_cast<float>(lCurrentVertex[0]);
				curPosValue[1] = static_cast<float>(lCurrentVertex[1]);
				curPosValue[2] = static_cast<float>(lCurrentVertex[2]);

				positions.addValue(curPosValue);

				// Save vertex color
				if(hasColor)
				{
					int lColorIndex = lIndex;
					if (lColorElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						lColorIndex = lColorElement->GetIndexArray().GetAt(lIndex);

					FbxColor lCurrentColor = lColorElement->GetDirectArray().GetAt(lColorIndex);

					Color curColorValue;
					curColorValue[0] = static_cast<float>(lCurrentColor[0]);
					curColorValue[1] = static_cast<float>(lCurrentColor[1]);
					curColorValue[2] = static_cast<float>(lCurrentColor[2]);
					curColorValue[3] = static_cast<float>(lCurrentColor[3]);

					UINT32 color32 = curColorValue.getAsRGBA();
					colors.addValue(color32);
				}

				// Save the normal.
				if (hasNormal)
				{
					int lNormalIndex = lIndex;
					if (lNormalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndex);

					lCurrentNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);

					Vector3 curNormalValue;
					curNormalValue[0] = static_cast<float>(lCurrentNormal[0]);
					curNormalValue[1] = static_cast<float>(lCurrentNormal[1]);
					curNormalValue[2] = static_cast<float>(lCurrentNormal[2]);

					normals.addValue(curNormalValue);
				}

				// Save the tangent.
				if (hasTangent)
				{
					int lTangentIndex = lIndex;
					if (lTangentElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						lTangentIndex = lTangentElement->GetIndexArray().GetAt(lIndex);

					FbxVector4 lCurrentTangent = lTangentElement->GetDirectArray().GetAt(lTangentIndex);

					Vector3 curTangentValue;
					curTangentValue[0] = static_cast<float>(lCurrentTangent[0]);
					curTangentValue[1] = static_cast<float>(lCurrentTangent[1]);
					curTangentValue[2] = static_cast<float>(lCurrentTangent[2]);

					tangents.addValue(curTangentValue);
				}

				// Save the bitangent.
				if (hasBitangent)
				{
					int lBitangentIndex = lIndex;
					if (lBitangentElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						lBitangentIndex = lBitangentElement->GetIndexArray().GetAt(lIndex);

					FbxVector4 lCurrentBitangent = lBitangentElement->GetDirectArray().GetAt(lBitangentIndex);

					Vector3 curBitangentValue;
					curBitangentValue[0] = static_cast<float>(lCurrentBitangent[0]);
					curBitangentValue[1] = static_cast<float>(lCurrentBitangent[1]);
					curBitangentValue[2] = static_cast<float>(lCurrentBitangent[2]);

					bitangents.addValue(curBitangentValue);
				}

				// Save the UV.
				if (hasUV0)
				{
					int lUVIndex = lIndex;
					if (lUVElement0->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						lUVIndex = lUVElement0->GetIndexArray().GetAt(lIndex);

					lCurrentUV = lUVElement0->GetDirectArray().GetAt(lUVIndex);

					Vector2 curUV0Value;
					curUV0Value[0] = static_cast<float>(lCurrentUV[0]);
					curUV0Value[1] = static_cast<float>(lCurrentUV[1]);

					uv0.addValue(curUV0Value);
				}

				if (hasUV1)
				{
					int lUVIndex = lIndex;
					if (lUVElement1->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
						lUVIndex = lUVElement1->GetIndexArray().GetAt(lIndex);

					lCurrentUV = lUVElement1->GetDirectArray().GetAt(lUVIndex);

					Vector2 curUV1Value;
					curUV1Value[0] = static_cast<float>(lCurrentUV[0]);
					curUV1Value[1] = static_cast<float>(lCurrentUV[1]);

					uv1.addValue(curUV1Value);
				}
			}
		}

		Vector<UINT32>::type indexOffsetPerSubmesh;
		indexOffsetPerSubmesh.resize(subMeshes.size(), 0);

		Vector<UINT32*>::type indices;
		indices.resize(subMeshes.size());

		for(UINT32 i = 0; i < (UINT32)indices.size(); i++)
		{
			indices[i] = meshData->getIndices32() + subMeshes[i].indexOffset;
		}

		UINT32 lVertexCount = 0;
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

					Vector3 curPosValue;
					curPosValue[0] = static_cast<float>(lCurrentVertex[0]);
					curPosValue[1] = static_cast<float>(lCurrentVertex[1]);
					curPosValue[2] = static_cast<float>(lCurrentVertex[2]);

					positions.addValue(curPosValue);

					if(hasColor)
					{
						int lColorIndex = lIndexOffset + lVerticeIndex; // TODO - Is this right?
						if (lColorElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
							lColorIndex = lColorElement->GetIndexArray().GetAt(lColorIndex);

						FbxColor lCurrentColor = lColorElement->GetDirectArray().GetAt(lColorIndex);

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
						mesh->GetPolygonVertexNormal(lPolygonIndex, lVerticeIndex, lCurrentNormal);

						Vector3 curNormalValue;
						curNormalValue[0] = static_cast<float>(lCurrentNormal[0]);
						curNormalValue[1] = static_cast<float>(lCurrentNormal[1]);
						curNormalValue[2] = static_cast<float>(lCurrentNormal[2]);

						normals.addValue(curNormalValue);
					}

					if (hasTangent)
					{
						int lTangentIndex = lIndexOffset + lVerticeIndex; // TODO - Is this right?
						if (lTangentElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
							lTangentIndex = lTangentElement->GetIndexArray().GetAt(lTangentIndex);

						FbxVector4 lCurrentTangent = lTangentElement->GetDirectArray().GetAt(lTangentIndex);

						Vector3 curTangentValue;
						curTangentValue[0] = static_cast<float>(lCurrentTangent[0]);
						curTangentValue[1] = static_cast<float>(lCurrentTangent[1]);
						curTangentValue[2] = static_cast<float>(lCurrentTangent[2]);

						tangents.addValue(curTangentValue);
					}

					if (hasBitangent)
					{
						int lBitangentIndex = lIndexOffset + lVerticeIndex; // TODO - Is this right?
						if (lBitangentElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
							lBitangentIndex = lBitangentElement->GetIndexArray().GetAt(lBitangentIndex);

						FbxVector4 lCurrentBitangent = lBitangentElement->GetDirectArray().GetAt(lBitangentIndex);

						Vector3 curBitangentValue;
						curBitangentValue[0] = static_cast<float>(lCurrentBitangent[0]);
						curBitangentValue[1] = static_cast<float>(lCurrentBitangent[1]);
						curBitangentValue[2] = static_cast<float>(lCurrentBitangent[2]);

						bitangents.addValue(curBitangentValue);
					}

					if (hasUV0)
					{
						mesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUVName0, lCurrentUV);

						Vector2 curUV0Value;
						curUV0Value[0] = static_cast<float>(lCurrentUV[0]);
						curUV0Value[1] = static_cast<float>(lCurrentUV[1]);

						uv0.addValue(curUV0Value);
					}

					if (hasUV1)
					{
						mesh->GetPolygonVertexUV(lPolygonIndex, lVerticeIndex, lUVName1, lCurrentUV);

						Vector2 curUV1Value;
						curUV1Value[0] = static_cast<float>(lCurrentUV[0]);
						curUV1Value[1] = static_cast<float>(lCurrentUV[1]);

						uv1.addValue(curUV1Value);
					}
				}

				++lVertexCount;
			}

			indexOffsetPerSubmesh[lMaterialIndex] += 3;
		}

		return meshData;
	}
}