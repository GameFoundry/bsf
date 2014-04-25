#pragma once

#include "CmFBXPrerequisites.h"
#include "CmSpecificImporter.h"
#include "CmImporter.h"
#include "CmSubMesh.h"

#define FBXSDK_NEW_API
#include <fbxsdk.h>

namespace BansheeEngine
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
				importer = cm_new<FBXImporter>();
				Importer::instance()._registerAssetImporter(importer);
			}
		}

		/** Inherited from SpecificImporter */
		virtual bool isExtensionSupported(const WString& ext) const;

		/** Inherited from SpecificImporter */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const; 

		/** Inherited from SpecificImporter */
		virtual ResourcePtr import(const WString& filePath, ConstImportOptionsPtr importOptions);
	private:
		Vector<WString>::type mExtensions;

		void startUpSdk(FbxManager*& manager, FbxScene*& scene);
		void shutDownSdk(FbxManager* manager);

		void loadScene(FbxManager* manager, FbxScene* scene, const WString& filePath);
		MeshDataPtr parseScene(FbxManager* manager, FbxScene* scene, Vector<SubMesh>::type& subMeshes);

		MeshDataPtr parseMesh(FbxMesh* mesh, Vector<SubMesh>::type& subMeshes, bool createTangentsIfMissing = true);
	};
}