#pragma once

#include "BsFBXPrerequisites.h"
#include "BsSpecificImporter.h"
#include "BsSubMesh.h"
#include "BsFBXImportData.h"

#define FBX_IMPORT_MAX_UV_LAYERS 2

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
		virtual bool isExtensionSupported(const WString& ext) const override;

		/**
		 * @copydoc	SpecificImporter::isMagicNumberSupported
		 */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const override;

		/**
		 * @copydoc	SpecificImporter::import
		 */
		virtual ResourcePtr import(const Path& filePath, ConstImportOptionsPtr importOptions) override;

		/**
		 * @copydoc	SpecificImporter::createImportOptions
		 */
		ImportOptionsPtr createImportOptions() const override;
	private:
		/**
		 * @brief	Starts up FBX SDK. Must be called before any other operations.
		 *			Outputs an FBX manager and FBX scene instances you should use in
		 *			further operations.
		 */
		bool startUpSdk(FbxScene*& scene);

		/**
		 * @brief	Shuts down FBX SDK. Must be called after any other operations.
		 */
		void shutDownSdk();

		/**
		 * @brief	Loads the data from the file at the provided path into the provided FBX scene. 
		 *			Throws exception if data cannot be loaded.
		 */
		bool loadFBXFile(FbxScene* scene, const Path& filePath);

		/**
		 * @brief	Parses an FBX scene. Find all meshes in the scene and returns mesh data object
		 *			containing all vertices, indexes and other mesh information. Also outputs
		 *			a sub-mesh array that allows you locate specific sub-meshes within the returned
		 *			mesh data object. If requested animation and blend shape data is output as well.
		 */
		void parseScene(FbxScene* scene, const FBXImportOptions& options, FBXImportScene& outputScene);

		/**
		 * @brief	Parses an FBX mesh. Converts it from FBX SDK format into a mesh data object containing
		 *			one or multiple sub-meshes.
		 */
		void parseMesh(FbxMesh* mesh, FBXImportNode* parentNode, const FBXImportOptions& options, FBXImportScene& outputScene);

		/**
		 * @brief	Imports blend shapes for all the meshes that are part of the scene.
		 */
		void importBlendShapes(FBXImportScene& scene, const FBXImportOptions& options);

		/**
		 * @brief	Parses a single FBX blend shape frame. Converts it from FBX SDK format into a 
		 *			shape data object containing position and tangent frame.
		 */
		void importBlendShapeFrame(FbxShape* shape, const FBXImportMesh& mesh, const FBXImportOptions& options, FBXBlendShapeFrame& outFrame);

		/**
		 * @brief	Imports skinning information and bones for all meshes.
		 */
		void importSkin(FBXImportScene& scene);

		/**
		 * @brief	Imports skinning information and bones for the specified mesh.
		 */
		void importSkin(FBXImportScene& scene, FbxSkin* skin, FBXImportMesh& mesh);

		/**
		 * @brief	Imports all bone and blend shape animations from the FBX.
		 */
		void importAnimations(FbxScene* scene, FBXImportOptions& importOptions, FBXImportScene& importScene);

		/**
		 * @brief	Imports all animations for the specified animation layer and outputs them in the provided clip.
		 *			Child nodes will be iterated recursively.
		 */
		void importAnimations(FbxAnimLayer* layer, FbxNode* node, FBXImportOptions& importOptions, 
			FBXAnimationClip& clip, FBXImportScene& importScene);

		/**
		 * @brief	Converts a single FBX animation curve into an engine curve format, resampling it if
		 *			necessary.
		 */
		void importCurve(FbxAnimCurve* fbxCurve, FBXImportOptions& importOptions, FBXAnimationCurve& curve, float start, float end);

		/**
		 * @brief	Converts a set of curves containing rotation in euler angles into a set of curves using
		 *			quaternion rotation.
		 */
		void eulerToQuaternionCurves(FBXAnimationCurve(&eulerCurves)[3], FBXAnimationCurve(&quatCurves)[4]);

		/**
		 * @brief	Converts all the meshes from per-index attributes to per-vertex attributes.
		 *
		 * @note	This method will replace all meshes in the scene with new ones, and delete old ones
		 *			so be sure not to keep any mesh references.
		 */
		void splitMeshVertices(FBXImportScene& scene);

		/**
		 * @brief	Converts the mesh data from the imported FBX scene into mesh data that can be used
		 *			for initializing a mesh.
		 */
		MeshDataPtr generateMeshData(const FBXImportScene& scene, const FBXImportOptions& options, Vector<SubMesh>& subMeshes);

		/**
		 * @brief	Creates an internal representation of an FBX node from an FbxNode object.
		 */
		FBXImportNode* createImportNode(FBXImportScene& scene, FbxNode* fbxNode, FBXImportNode* parent);

	private:
		Vector<WString> mExtensions;
		FbxManager* mFBXManager;
	};
}