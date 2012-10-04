#pragma once

#include "CmFBXPrerequisites.h"
#include "CmSpecificImporter.h"
#include "CmImporter.h"

#define FBXSDK_NEW_API
#include <fbxsdk.h>

namespace CamelotEngine
{
	class CM_FBX_EXPORT FBXImporter : public SpecificImporter
	{
		struct FBXSubMeshData
		{
			FBXSubMeshData():
				indexOffset(0), indexCount(0)
			{ }

			int indexOffset;
			int indexCount;
		};

		struct FBXMeshData
		{
			FBXMeshData():
				vertex(nullptr),
				normal(nullptr),
				tangent(nullptr),
				bitangent(nullptr),
				uv0(nullptr),
				uv1(nullptr),
				index(nullptr),
				indexCount(0),
				vertexCount(0)
			{ }

			Vector3* vertex;
			Vector3* normal;
			Vector3* tangent;
			Vector3* bitangent;
			Vector2* uv0;
			Vector2* uv1;
			int* index;

			int indexCount;
			int vertexCount;

			vector<FBXSubMeshData>::type subMeshes;
		};

	public:
		FBXImporter();
		virtual ~FBXImporter();

		/**
		 * @brief	Should only be called by the plugin when its being loaded.
		 */
		static void startUp()
		{
			static FBXImporter* importer = nullptr;
			if(importer == nullptr)
			{
				importer = new FBXImporter();
				Importer::instance().registerAssetImporter(importer);
			}
		}

		/** Inherited from SpecificImporter */
		virtual bool isExtensionSupported(const String& ext) const;

		/** Inherited from SpecificImporter */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const; 

		/** Inherited from SpecificImporter */
		virtual ResourcePtr import(const String& filePath);
	private:
		vector<String>::type mExtensions;

		void startUpSdk(FbxManager*& manager, FbxScene*& scene);
		void shutDownSdk(FbxManager* manager);

		void loadScene(FbxManager* manager, FbxScene* scene, const String& filePath);
		FBXMeshData* parseScene(FbxManager* manager, FbxScene* scene);

		FBXMeshData* parseMesh(FbxMesh* mesh, bool createTangentsIfMissing = true);
	};
}