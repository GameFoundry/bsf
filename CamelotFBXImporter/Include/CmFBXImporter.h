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
		virtual BaseResourceHandle import(const String& filePath, ConstImportOptionsPtr importOptions);
	private:
		vector<String>::type mExtensions;

		void startUpSdk(FbxManager*& manager, FbxScene*& scene);
		void shutDownSdk(FbxManager* manager);

		void loadScene(FbxManager* manager, FbxScene* scene, const String& filePath);
		MeshDataPtr parseScene(FbxManager* manager, FbxScene* scene);

		MeshDataPtr parseMesh(FbxMesh* mesh, bool createTangentsIfMissing = true);
		MeshDataPtr mergeMeshData(vector<MeshDataPtr>::type meshes);

		void initDeclarationForMeshData(MeshDataPtr meshData);
	};
}