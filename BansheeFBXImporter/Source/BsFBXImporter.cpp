#include "BsFBXImporter.h"
#include "BsResource.h"
#include "BsCoreApplication.h"
#include "BsDebug.h"
#include "BsDataStream.h"
#include "BsMeshData.h"
#include "BsMesh.h"
#include "BsVector2.h"
#include "BsVector3.h"
#include "BsVector4.h"
#include "BsQuaternion.h"
#include "BsVertexDataDesc.h"
#include "BsFBXUtility.h"
#include <BsMeshImportOptions.h>

namespace BansheeEngine
{
	Vector4 FBXToNativeType(const FbxVector4& value)
	{
		Vector4 native;
		native.x = (float)value[0];
		native.y = (float)value[1];
		native.z = (float)value[2];
		native.w = (float)value[3];

		return native;
	}

	Vector3 FBXToNativeType(const FbxDouble3& value)
	{
		Vector3 native;
		native.x = (float)value[0];
		native.y = (float)value[1];
		native.z = (float)value[2];

		return native;
	}

	Vector2 FBXToNativeType(const FbxVector2& value)
	{
		Vector2 native;
		native.x = (float)value[0];
		native.y = (float)value[1];

		return native;
	}

	RGBA FBXToNativeType(const FbxColor& value)
	{
		Color native;
		native.r = (float)value[0];
		native.g = (float)value[1];
		native.b = (float)value[2];
		native.a = (float)value[3];

		return native.getAsRGBA();
	}

	FbxSurfaceMaterial* FBXToNativeType(FbxSurfaceMaterial* const& value)
	{
		return value;
	}

	int FBXToNativeType(const int & value)
	{
		return value;
	}

	FBXImporter::FBXImporter()
		:SpecificImporter(), mFBXManager(nullptr)
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

	ImportOptionsPtr FBXImporter::createImportOptions() const
	{
		return bs_shared_ptr<MeshImportOptions, PoolAlloc>();
	}

	ResourcePtr FBXImporter::import(const Path& filePath, ConstImportOptionsPtr importOptions)
	{
		FbxScene* fbxScene = nullptr;

		if (!startUpSdk(fbxScene))
			return nullptr;

		if (!loadFBXFile(fbxScene, filePath))
			return nullptr;

		const MeshImportOptions* meshImportOptions = static_cast<const MeshImportOptions*>(importOptions.get());
		FBXImportOptions fbxImportOptions;
		fbxImportOptions.importNormals = meshImportOptions->getImportNormals();
		fbxImportOptions.importTangents = meshImportOptions->getImportTangents();
		fbxImportOptions.importAnimation = meshImportOptions->getImportAnimation();
		fbxImportOptions.importBlendShapes = meshImportOptions->getImportBlendShapes();
		fbxImportOptions.importSkin = meshImportOptions->getImportSkin();
		fbxImportOptions.importScale = meshImportOptions->getImportScale();

		FBXImportScene importedScene;
		parseScene(fbxScene, fbxImportOptions, importedScene);
		importBlendShapes(importedScene, fbxImportOptions);
		splitMeshVertices(importedScene);
		
		Vector<SubMesh> subMeshes;
		MeshDataPtr meshData = generateMeshData(importedScene, fbxImportOptions, subMeshes);

		// TODO - Later: Optimize mesh: Remove bad and degenerate polygons, optimize for vertex cache

		shutDownSdk();

		INT32 usage = MU_STATIC;
		if (meshImportOptions->getCPUReadable())
			usage |= MU_CPUCACHED;

		MeshPtr mesh = Mesh::_createPtr(meshData, subMeshes, usage);

		WString fileName = filePath.getWFilename(false);
		mesh->setName(fileName);

		return mesh;
	}

	bool FBXImporter::startUpSdk(FbxScene*& scene)
	{
		mFBXManager = FbxManager::Create();
		if (mFBXManager == nullptr)
		{
			LOGERR("FBX import failed: FBX SDK failed to initialize. FbxManager::Create() failed.");
			return false;
		}

		FbxIOSettings* ios = FbxIOSettings::Create(mFBXManager, IOSROOT);
		mFBXManager->SetIOSettings(ios);

		scene = FbxScene::Create(mFBXManager, "Import Scene");
		if (scene == nullptr)
		{
			LOGWRN("FBX import failed: Failed to create FBX scene.");
			return false;
		}

		return true;
	}

	void FBXImporter::shutDownSdk()
	{
		mFBXManager->Destroy();
		mFBXManager = nullptr;
	}

	bool FBXImporter::loadFBXFile(FbxScene* scene, const Path& filePath)
	{
		int lFileMajor, lFileMinor, lFileRevision;
		int lSDKMajor,  lSDKMinor,  lSDKRevision;
		FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

		FbxImporter* importer = FbxImporter::Create(mFBXManager, "");
		bool importStatus = importer->Initialize(filePath.toString().c_str(), -1, mFBXManager->GetIOSettings());
		
		importer->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

		if(!importStatus)
		{
			LOGERR("FBX import failed: Call to FbxImporter::Initialize() failed.\n" +
				String("Error returned: %s\n\n") + String(importer->GetStatus().GetErrorString()));
			return false;
		}

		mFBXManager->GetIOSettings()->SetBoolProp(IMP_FBX_TEXTURE, false);
		mFBXManager->GetIOSettings()->SetBoolProp(IMP_FBX_GOBO, false);

		importStatus = importer->Import(scene);

		if(!importStatus)
		{
			importer->Destroy();
			
			LOGERR("FBX import failed: Call to FbxImporter::Initialize() failed.\n" +
				String("Error returned: %s\n\n") + String(importer->GetStatus().GetErrorString()));
			return false;
		}

		FbxAxisSystem fileCoordSystem = scene->GetGlobalSettings().GetAxisSystem();
		FbxAxisSystem bsCoordSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
		if (fileCoordSystem != bsCoordSystem)
			bsCoordSystem.ConvertScene(scene);

		importer->Destroy();
		return true;
	}

	void FBXImporter::parseScene(FbxScene* scene, const FBXImportOptions& options, FBXImportScene& outputScene)
	{
		outputScene.rootNode = createImportNode(outputScene, scene->GetRootNode(), nullptr);

		Stack<FbxNode*> todo;
		todo.push(scene->GetRootNode());

		while(!todo.empty())
		{
			FbxNode* curNode = todo.top();
			FBXImportNode* curImportNode = outputScene.nodeMap[curNode];
			todo.pop();

			const char* name = curNode->GetName();

			FbxNodeAttribute* attrib = curNode->GetNodeAttribute();
			if(attrib != nullptr)
			{
				FbxNodeAttribute::EType attribType = attrib->GetAttributeType();

				switch(attribType)
				{
				case FbxNodeAttribute::eNurbs:
				case FbxNodeAttribute::eNurbsSurface:
				case FbxNodeAttribute::ePatch:
				{
					FbxGeometryConverter geomConverter(mFBXManager);
					attrib = geomConverter.Triangulate(attrib, true);

					if (attrib->GetAttributeType() == FbxNodeAttribute::eMesh)
					{
						FbxMesh* mesh = static_cast<FbxMesh*>(attrib);
						mesh->RemoveBadPolygons();

						parseMesh(mesh, curImportNode, options, outputScene);
					}
				}
					break;
				case FbxNodeAttribute::eMesh:
					{
						FbxMesh* mesh = static_cast<FbxMesh*>(attrib);
						mesh->RemoveBadPolygons();

						if(!mesh->IsTriangleMesh())
						{
							FbxGeometryConverter geomConverter(mFBXManager);
							geomConverter.Triangulate(mesh, true);
							attrib = curNode->GetNodeAttribute();
							mesh = static_cast<FbxMesh*>(attrib);
						}

						parseMesh(mesh, curImportNode, options, outputScene);
					}
					break;
				}
			}

			for (int i = 0; i < curNode->GetChildCount(); i++)
			{
				FbxNode* childNode = curNode->GetChild(i);
				createImportNode(outputScene, childNode, curImportNode);

				todo.push(childNode);
			}
		}

		// TODO - Parse skin
		// TODO - Parse animation
	}

	FBXImportNode* FBXImporter::createImportNode(FBXImportScene& scene, FbxNode* fbxNode, FBXImportNode* parent)
	{
		FBXImportNode* node = bs_new<FBXImportNode>();

		Vector3 translation = FBXToNativeType(fbxNode->LclTranslation.Get());
		Vector3 rotationEuler = FBXToNativeType(fbxNode->LclRotation.Get());
		Vector3 scale = FBXToNativeType(fbxNode->LclScaling.Get());

		Quaternion rotation((Radian)rotationEuler.x, (Radian)rotationEuler.y, (Radian)rotationEuler.z);

		node->localTransform.setTRS(translation, rotation, scale);

		if (parent != nullptr)
		{
			node->worldTransform = node->localTransform * parent->worldTransform;

			parent->children.push_back(node);
		}
		else
			node->worldTransform = node->localTransform;

		scene.nodeMap.insert(std::make_pair(fbxNode, node));

		return node;
	}

	void FBXImporter::splitMeshVertices(FBXImportScene& scene)
	{
		Vector<FBXImportMesh*> splitMeshes;

		for (auto& mesh : scene.meshes)
		{
			FBXImportMesh* splitMesh = bs_new<FBXImportMesh>();
			splitMesh->fbxMesh = mesh->fbxMesh;
			splitMesh->referencedBy = mesh->referencedBy;

			FBXUtility::splitVertices(*mesh, *splitMesh);
			FBXUtility::flipWindingOrder(*splitMesh);
			splitMeshes.push_back(splitMesh);

			bs_delete(mesh);
		}

		scene.meshes = splitMeshes;
	}

	MeshDataPtr FBXImporter::generateMeshData(const FBXImportScene& scene, const FBXImportOptions& options, Vector<SubMesh>& outputSubMeshes)
	{
		Matrix4 importScale = Matrix4::scaling(options.importScale);

		Vector<MeshDataPtr> allMeshData;
		Vector<Vector<SubMesh>> allSubMeshes;

		for (auto& mesh : scene.meshes)
		{
			Vector<Vector<UINT32>> indicesPerMaterial;
			for (UINT32 i = 0; i < (UINT32)mesh->indices.size(); i++)
			{
				while (mesh->materials[i] >= indicesPerMaterial.size())
					indicesPerMaterial.push_back(Vector<UINT32>());

				indicesPerMaterial[mesh->materials[i]].push_back(mesh->indices[i]);
			}

			UINT32* orderedIndices = (UINT32*)bs_alloc((UINT32)mesh->indices.size() * sizeof(UINT32));
			Vector<SubMesh> subMeshes;
			UINT32 currentIndex = 0;

			for (auto& subMeshIndices : indicesPerMaterial)
			{
				UINT32 indexCount = (UINT32)subMeshIndices.size();
				UINT32* dest = orderedIndices + currentIndex;
				memcpy(dest, subMeshIndices.data(), indexCount * sizeof(UINT32));

				subMeshes.push_back(SubMesh(currentIndex, indexCount, DOT_TRIANGLE_LIST));

				currentIndex += indexCount;
			}

			VertexDataDescPtr vertexDesc = bs_shared_ptr<VertexDataDesc>();
			vertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);

			size_t numVertices = mesh->positions.size();
			bool hasColors = mesh->colors.size() == numVertices;
			bool hasNormals = mesh->normals.size() == numVertices;

			if (hasColors)
				vertexDesc->addVertElem(VET_COLOR, VES_COLOR);

			bool hasTangents = false;
			if (hasNormals)
			{
				vertexDesc->addVertElem(VET_FLOAT3, VES_NORMAL);

				if (mesh->tangents.size() == numVertices &&
					mesh->bitangents.size() == numVertices)
				{
					vertexDesc->addVertElem(VET_FLOAT4, VES_TANGENT);
					hasTangents = true;
				}
			}

			int UVIdx = 0;
			for (UINT32 i = 0; i < FBX_IMPORT_MAX_UV_LAYERS; i++)
			{
				if (mesh->UV[i].size() == numVertices)
				{
					vertexDesc->addVertElem(VET_FLOAT2, VES_TEXCOORD, UVIdx++);
				}
			}

			UINT32 numIndices = (UINT32)mesh->indices.size();
			for (auto& node : mesh->referencedBy)
			{
				Matrix4 worldTransform = node->worldTransform * importScale;
				Matrix4 worldTransformIT = worldTransform.transpose();
				worldTransformIT = worldTransformIT.inverse();

				MeshDataPtr meshData = bs_shared_ptr<MeshData>((UINT32)numVertices, numIndices, vertexDesc);

				// Copy indices
				UINT32* indices = meshData->getIndices32();
				memcpy(indices, mesh->indices.data(), numIndices * sizeof(UINT32));

				// Copy & transform positions
				auto posIter = meshData->getVec3DataIter(VES_POSITION, 0);

				for (auto& position : mesh->positions)
				{
					Vector3 tfrmdValue = worldTransform.multiplyAffine((Vector3)position);
					posIter.addValue(tfrmdValue);
				}

				// Copy & transform normals
				if (hasNormals)
				{
					auto normalIter = meshData->getVec3DataIter(VES_NORMAL, 0);

					// Copy, convert & transform tangents & bitangents
					if (hasTangents)
					{
						auto tangentIter = meshData->getVec4DataIter(VES_TANGENT, 0);

						for (UINT32 i = 0; i < (UINT32)numVertices; i++)
						{
							Vector3 normal = (Vector3)mesh->normals[i];
							normal = worldTransformIT.multiplyAffine(normal);
							normalIter.addValue(normal);

							Vector3 tangent = (Vector3)mesh->tangents[i];
							tangent = worldTransformIT.multiplyAffine(tangent);

							Vector3 bitangent = (Vector3)mesh->bitangents[i];
							bitangent = worldTransformIT.multiplyAffine(bitangent);

							Vector3 engineBitangent = Vector3::cross(normal, tangent);
							float sign = Vector3::dot(engineBitangent, bitangent);

							Vector4 combinedTangent(tangent.x, tangent.y, tangent.z, sign > 0 ? 1.0f : -1.0f);
							tangentIter.addValue(combinedTangent);
						}
					}
					else // Just normals
					{
						for (auto& normal : mesh->normals)
						{
							Vector3 tfrmdValue = worldTransformIT.multiplyAffine((Vector3)normal);
							normalIter.addValue(tfrmdValue);
						}
					}
				}

				// Copy colors
				if (hasColors)
				{
					auto colorIter = meshData->getDWORDDataIter(VES_COLOR, 0);

					for (auto& color : mesh->colors)
						colorIter.addValue(color);
				}

				// Copy UV
				int writeUVIDx = 0;
				for (auto& uvLayer : mesh->UV)
				{
					if (uvLayer.size() == numVertices)
					{
						auto uvIter = meshData->getVec2DataIter(VES_TEXCOORD, writeUVIDx);

						for (auto& uv : uvLayer)
						{
							uv.y = 1.0f - uv.y;
							uvIter.addValue(uv);
						}

						writeUVIDx++;
					}
				}

				allMeshData.push_back(meshData);
				allSubMeshes.push_back(subMeshes);
			}
		}

		if (allMeshData.size() > 1)
		{
			return MeshData::combine(allMeshData, allSubMeshes, outputSubMeshes);
		}
		else if (allMeshData.size() == 1)
		{
			outputSubMeshes = allSubMeshes[0];
			return allMeshData[0];
		}

		return nullptr;
	}

	template<class TFBX, class TNative>
	class FBXDirectIndexer
	{
	public:
		FBXDirectIndexer(const FbxLayerElementTemplate<TFBX>& layer)
			:mElementArray(layer.GetDirectArray()),
			mElementCount(mElementArray.GetCount())
		{}

		bool get(int index, TNative& output) const
		{
			if (index < 0 || index >= mElementCount)
				return false;

			output = FBXToNativeType(mElementArray.GetAt(index));
			return true;
		}

		bool isEmpty() const
		{
			return mElementCount == 0;
		}

	private:
		const FbxLayerElementArrayTemplate<TFBX>& mElementArray;
		int mElementCount;
	};

	template<class TFBX, class TNative>
	class FBXIndexIndexer
	{
	public:
		FBXIndexIndexer(const FbxLayerElementTemplate<TFBX>& layer)
			:mElementArray(layer.GetDirectArray()),
			mIndexArray(layer.GetIndexArray()),
			mElementCount(mElementArray.GetCount()),
			mIndexCount(mIndexArray.GetCount())
		{}

		bool get(int index, TNative& output) const
		{
			if (index < 0 || index >= mIndexCount)
				return false;

			int actualIndex = mIndexArray.GetAt(index);

			if (actualIndex < 0 || actualIndex >= mElementCount)
				return false;

			output = FBXToNativeType(mElementArray.GetAt(actualIndex));
			return true;
		}

		bool isEmpty() const
		{
			return mElementCount == 0 || mIndexCount == 0;
		}

	private:
		const FbxLayerElementArrayTemplate<TFBX>& mElementArray;
		const FbxLayerElementArrayTemplate<int>& mIndexArray;
		int mElementCount;
		int mIndexCount;
	};

	template<class TFBX, class TNative, class TIndexer>
	void readLayerData(FbxLayerElementTemplate<TFBX>& layer, Vector<TNative>& output, const Vector<int>& indices)
	{
		TIndexer indexer(layer);
		if (indexer.isEmpty())
			return;

		output.resize(indices.size());

		FbxLayerElement::EMappingMode mappingMode = layer.GetMappingMode();

		UINT32 indexCount = (UINT32)indices.size();
		switch (mappingMode)
		{
		case FbxLayerElement::eByControlPoint:
			for (UINT32 i = 0; i < indexCount; i++)
			{
				int index = indices[i];
				indexer.get(index, output[i]);
			}
			break;
		case FbxLayerElement::eByPolygonVertex:
			for (UINT32 i = 0; i < indexCount; i++)
				indexer.get(i, output[i]);
			break;
		case FbxLayerElement::eByPolygon:
			// We expect mesh to be triangulated here
		{
			UINT32 polygonCount = indexCount / 3;
			UINT32 index = 0;

			for (UINT32 i = 0; i < polygonCount; i++)
			{
				TNative value;
				indexer.get(i, value);

				output[index++] = value;
				output[index++] = value;
				output[index++] = value;
			}
		}
			break;
		case FbxLayerElement::eAllSame:
		{
			TNative value;
			indexer.get(0, value);

			for (UINT32 i = 0; i < indexCount; i++)
				output[i] = value;
		}
			break;
		default:
			LOGWRN("FBX Import: Unsupported layer mapping mode.");
			break;
		}
	}

	template<class TFBX, class TNative>
	void readLayerData(FbxLayerElementTemplate<TFBX>& layer, Vector<TNative>& output, const Vector<int>& indices)
	{
		FbxLayerElement::EReferenceMode refMode = layer.GetReferenceMode();

		if (refMode == FbxLayerElement::eDirect)
			readLayerData<TFBX, TNative, FBXDirectIndexer<TFBX, TNative> >(layer, output, indices);
		else if (refMode == FbxLayerElement::eIndexToDirect)
			readLayerData<TFBX, TNative, FBXIndexIndexer<TFBX, TNative> >(layer, output, indices);
		else
			LOGWRN("FBX Import: Unsupported layer reference mode.");
	}

	void FBXImporter::parseMesh(FbxMesh* mesh, FBXImportNode* parentNode, const FBXImportOptions& options, FBXImportScene& outputScene)
	{
		// Check if valid
		if (!mesh->IsTriangleMesh())
			return;

		UINT32 vertexCount = mesh->GetControlPointsCount();
		UINT32 triangleCount = mesh->GetPolygonCount();

		if (vertexCount == 0 || triangleCount == 0)
			return;

		// Register in global mesh array
		FBXImportMesh* importMesh = nullptr;

		auto iterFindMesh = outputScene.meshMap.find(mesh);
		if (iterFindMesh != outputScene.meshMap.end())
		{
			UINT32 meshIdx = iterFindMesh->second;
			outputScene.meshes[meshIdx]->referencedBy.push_back(parentNode);

			return;
		}
		else
		{
			importMesh = bs_new<FBXImportMesh>();
			outputScene.meshes.push_back(importMesh);

			importMesh->referencedBy.push_back(parentNode);
			importMesh->fbxMesh = mesh;
			outputScene.meshMap[mesh] = (UINT32)outputScene.meshes.size() - 1;
		}

		// Import vertices
		importMesh->positions.resize(vertexCount);
		FbxVector4* controlPoints = mesh->GetControlPoints();

		for (UINT32 i = 0; i < vertexCount; i++)
			importMesh->positions[i] = FBXToNativeType(controlPoints[i]);

		// Import triangles
		UINT32 indexCount = triangleCount * 3;
		importMesh->indices.resize(indexCount);

		int* fbxIndices = mesh->GetPolygonVertices();
		importMesh->indices.assign(fbxIndices, fbxIndices + indexCount);

		// Import UVs
		Vector<FbxLayerElementUV*> fbxUVLayers;

		//// Search the diffuse layers first
		for (UINT32 i = 0; i < FBX_IMPORT_MAX_UV_LAYERS; i++)
		{
			FbxLayer* layer = mesh->GetLayer(i, FbxLayerElement::eUV);
			if (layer == nullptr)
				continue;

			for (int j = FbxLayerElement::eTextureDiffuse; j < FbxLayerElement::eTypeCount; j++)
			{
				FbxLayerElementUV* uvLayer = layer->GetUVs((FbxLayerElement::EType)j);
				if (uvLayer == nullptr)
					continue;

				fbxUVLayers.push_back(uvLayer);

				if (fbxUVLayers.size() == FBX_IMPORT_MAX_UV_LAYERS)
					break;
			}

			if (fbxUVLayers.size() == FBX_IMPORT_MAX_UV_LAYERS)
				break;
		}

		//// If there's room, search all others too
		if (fbxUVLayers.size() < FBX_IMPORT_MAX_UV_LAYERS)
		{
			UINT32 numLayers = mesh->GetLayerCount();
			for (UINT32 i = 0; i < numLayers; i++)
			{
				FbxLayer* layer = mesh->GetLayer(i);
				if (layer == nullptr)
					continue;

				for (int j = FbxLayerElement::eTextureDiffuse; j < FbxLayerElement::eTypeCount; j++)
				{
					FbxLayerElementUV* uvLayer = layer->GetUVs((FbxLayerElement::EType)j);
					if (uvLayer == nullptr)
						continue;

					auto iterFind = std::find(fbxUVLayers.begin(), fbxUVLayers.end(), uvLayer);
					if (iterFind != fbxUVLayers.end())
						continue;

					fbxUVLayers.push_back(uvLayer);

					if (fbxUVLayers.size() == FBX_IMPORT_MAX_UV_LAYERS)
						break;
				}

				if (fbxUVLayers.size() == FBX_IMPORT_MAX_UV_LAYERS)
					break;
			}
		}

		for (size_t i = 0; i < fbxUVLayers.size(); i++)
			readLayerData(*fbxUVLayers[i], importMesh->UV[i], importMesh->indices);

		FbxLayer* mainLayer = mesh->GetLayer(0);
		if (mainLayer != nullptr)
		{
			// Import colors
			if (mainLayer->GetVertexColors() != nullptr)
				readLayerData(*mainLayer->GetVertexColors(), importMesh->colors, importMesh->indices);

			// Import normals
			if (options.importNormals)
			{
				bool hasNormals = mainLayer->GetNormals() != nullptr;

				if (!hasNormals)
				{
					if (mainLayer->GetSmoothing() != nullptr)
					{
						FbxLayerElementSmoothing* smoothing = mainLayer->GetSmoothing();

						if (smoothing->GetMappingMode() == FbxLayerElement::eByEdge)
						{
							FbxGeometryConverter converter(mFBXManager);
							converter.ComputePolygonSmoothingFromEdgeSmoothing(mesh, 0);
						}

						readLayerData(*smoothing, importMesh->smoothingGroups, importMesh->indices);

						if (!importMesh->smoothingGroups.empty())
						{
							FBXUtility::normalsFromSmoothing(importMesh->positions, importMesh->indices, 
								importMesh->smoothingGroups, importMesh->normals);
							hasNormals = true;
						}
					}

					if (!hasNormals)
					{
						// TODO - Calculate normals
					}
				}
				else
					readLayerData(*mainLayer->GetNormals(), importMesh->normals, importMesh->indices);
			}

			// Import tangents
			if (options.importTangents)
			{
				bool hasTangents = mainLayer->GetTangents() != nullptr && mainLayer->GetBinormals() != nullptr;

				if (!hasTangents)
				{
					if (fbxUVLayers.size() > 0)
						hasTangents = mesh->GenerateTangentsData(0, false);
				}

				if (hasTangents)
				{
					readLayerData(*mainLayer->GetTangents(), importMesh->tangents, importMesh->indices);
					readLayerData(*mainLayer->GetBinormals(), importMesh->bitangents, importMesh->indices);
				}
				else
				{
					// TODO - Calculate tangent frame
				}
			}

			// Import material indexes
			if (mainLayer->GetMaterials() != nullptr)
			{
				Vector<FbxSurfaceMaterial*> fbxMaterials;

				readLayerData(*mainLayer->GetMaterials(), fbxMaterials, importMesh->indices);

				UnorderedMap<FbxSurfaceMaterial*, int> materialLookup;
				int nextMaterialIdx = 0;
				for (UINT32 i = 0; i < (UINT32)fbxMaterials.size(); i++)
				{
					auto iterFind = materialLookup.find(fbxMaterials[i]);

					int materialIdx = 0;
					if (iterFind != materialLookup.end())
						materialIdx = iterFind->second;
					else
					{
						materialIdx = nextMaterialIdx++;
						materialLookup[fbxMaterials[i]] = materialIdx;
					}

					importMesh->materials.push_back(materialIdx);
				}
			}
		}
	}

	void FBXImporter::importBlendShapes(FBXImportScene& scene, const FBXImportOptions& options)
	{
		for (auto& mesh : scene.meshes)
		{
			FbxMesh* fbxMesh = mesh->fbxMesh;

			UINT32 deformerCount = (UINT32)fbxMesh->GetDeformerCount(FbxDeformer::eBlendShape);
			for (UINT32 i = 0; i < deformerCount; i++)
			{
				FbxBlendShape* deformer = static_cast<FbxBlendShape*>(fbxMesh->GetDeformer(i, FbxDeformer::eBlendShape));

				UINT32 blendShapeChannelCount = (UINT32)deformer->GetBlendShapeChannelCount();
				for (UINT32 j = 0; j < blendShapeChannelCount; ++j)
				{
					FbxBlendShapeChannel* channel = deformer->GetBlendShapeChannel(j);
					double* weights = channel->GetTargetShapeFullWeights();

					UINT32 frameCount = channel->GetTargetShapeCount();
					if (frameCount == 0)
						continue;

					mesh->blendShapes.push_back(FBXBlendShape());
					FBXBlendShape& blendShape = mesh->blendShapes.back();
					blendShape.name = channel->GetName();
					blendShape.frames.resize(frameCount);

					for (UINT32 k = 0; k < frameCount; k++)
					{
						FbxShape* fbxShape = channel->GetTargetShape(k);

						FBXBlendShapeFrame& frame = blendShape.frames[k];
						frame.weight = (float)weights[k];

						importBlendShapeFrame(fbxShape, *mesh, options, frame);
					}
				}
			}
		}
	}

	void FBXImporter::importBlendShapeFrame(FbxShape* shape, const FBXImportMesh& mesh, const FBXImportOptions& options, FBXBlendShapeFrame& outFrame)
	{
		UINT32 vertexCount = (UINT32)shape->GetControlPointsCount();
		outFrame.positions.resize(vertexCount);
		FbxVector4* controlPoints = shape->GetControlPoints();

		for (UINT32 i = 0; i < vertexCount; i++)
			outFrame.positions[i] = FBXToNativeType(controlPoints[i]);

		FbxLayer* mainLayer = shape->GetLayer(0);
		if (options.importNormals)
		{
			bool hasNormals = mainLayer->GetNormals() != nullptr;

			if (!hasNormals)
			{
				if (!mesh.smoothingGroups.empty())
				{
					FBXUtility::normalsFromSmoothing(outFrame.positions, mesh.indices,
						mesh.smoothingGroups, outFrame.normals);
				}
				else
				{
					// TODO - Calculate normals
				}
			}
			else
				readLayerData(*mainLayer->GetNormals(), outFrame.normals, mesh.indices);
		}

		if (options.importTangents)
		{
			bool hasTangents = mainLayer->GetTangents() != nullptr && mainLayer->GetBinormals() != nullptr;

			if (hasTangents)
			{
				readLayerData(*mainLayer->GetTangents(), outFrame.tangents, mesh.indices);
				readLayerData(*mainLayer->GetBinormals(), outFrame.bitangents, mesh.indices);
			}
			else
			{
				// TODO - Calculate tangent frame
			}
		}
	}
}