#pragma once

#include "BsFBXPrerequisites.h"
#include "BsSpecificImporter.h"
#include "BsImporter.h"
#include "BsSubMesh.h"

#define FBXSDK_NEW_API
#include <fbxsdk.h>

namespace BansheeEngine
{
	/**
	 * @brief	Importer implementation that handles FBX/OBJ/DAE/3DS file import 
	 *			by using the FBX SDK.
	 */
	class BS_FBX_EXPORT FBXImporter : public SpecificImporter
	{
	public:
		FBXImporter();
		virtual ~FBXImporter();

		/**
		 * @copydoc	SpecificImporter::isExtensionSupported
		 */
		virtual bool isExtensionSupported(const WString& ext) const;

		/**
		 * @copydoc	SpecificImporter::isMagicNumberSupported
		 */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const; 

		/**
		 * @copydoc	SpecificImporter::import
		 */
		virtual ResourcePtr import(const Path& filePath, ConstImportOptionsPtr importOptions);
	private:
		/**
		 * @brief	Starts up FBX SDK. Must be called before any other operations.
		 *			Outputs an FBX manager and FBX scene instances you should use in
		 *			further operations.
		 */
		void startUpSdk(FbxManager*& manager, FbxScene*& scene);

		/**
		 * @brief	Shuts down FBX SDK. Must be called after any other operations.
		 */
		void shutDownSdk(FbxManager* manager);

		/**
		 * @brief	Loads the data from the file at the provided path into the provided FBX scene. 
		 *			Throws exception if data cannot be loaded.
		 */
		void loadScene(FbxManager* manager, FbxScene* scene, const Path& filePath);

		/**
		 * @brief	Parses an FBX scene. Find all meshes in the scene and returns mesh data object
		 *			containing all vertices, indexes and other mesh information. Also outputs
		 *			a sub-mesh array that allows you locate specific sub-meshes within the returned
		 *			mesh data object.
		 */
		MeshDataPtr parseScene(FbxManager* manager, FbxScene* scene, Vector<SubMesh>& subMeshes);

		/**
		 * @brief	Parses an FBX mesh. Converts it from FBX SDK format into a mesh data object containing
		 *			one or multiple sub-meshes.
		 */
		MeshDataPtr parseMesh(FbxMesh* mesh, Vector<SubMesh>& subMeshes, bool createTangentsIfMissing = true);

	private:
		Vector<WString> mExtensions;
	};
}