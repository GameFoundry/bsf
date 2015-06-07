#pragma once

#include "BsFBXPrerequisites.h"
#include "BsMatrix4.h"
#include "BsColor.h"
#include "BsVector2.h"
#include "BsVector4.h"
#include "BsSubMesh.h"

namespace BansheeEngine
{
	/**
	 * @brief	Options that control FBX import
	 */
	struct FBXImportOptions
	{
		bool importAnimation = true;
		bool importSkin = true;
		bool importBlendShapes = true;
		bool importNormals = true;
		bool importTangents = true;
		float importScale = 0.01f;
	};

	/**
	 * @brief	Represents a single node in the FBX transform hierarchy.
	 */
	struct FBXImportNode
	{
		~FBXImportNode();

		Matrix4 localTransform;
		Matrix4 worldTransform;
		FbxNode* fbxNode;

		Vector<FBXImportNode*> children;
	};

	/**
	 * @brief	Contains geometry from one blend shape frame.
	 */
	struct FBXBlendShapeFrame
	{
		Vector<Vector4> positions;
		Vector<Vector4> normals;
		Vector<Vector4> tangents;
		Vector<Vector4> bitangents;

		float weight;
	};

	/**
	 * @brief	Contains all geometry for a single blend shape.
	 */
	struct FBXBlendShape
	{
		String name;
		Vector<FBXBlendShapeFrame> frames;
	};

	/**
	 * @brief	Contains data about a single bone in a skinned mesh.
	 */
	struct FBXBone
	{
		FBXImportNode* node;
		Matrix4 bindPose;
	};

	/**
	 * @brief	Contains a set of bone weights and indices for a single
	 *			vertex, used in a skinned mesh.
	 */
	struct FBXBoneInfluence
	{
		FBXBoneInfluence()
		{
			for (UINT32 i = 0; i < FBX_IMPORT_MAX_BONE_INFLUENCES; i++)
			{
				weights[i] = 0.0f;
				indices[i] = -1;
			}
		}

		float weights[FBX_IMPORT_MAX_BONE_INFLUENCES];
		INT32 indices[FBX_IMPORT_MAX_BONE_INFLUENCES];
	};

	/**
	 * @brief	Imported mesh data.
	 */
	struct FBXImportMesh
	{
		FbxMesh* fbxMesh;

		Vector<int> indices;
		Vector<Vector4> positions;
		Vector<Vector4> normals;
		Vector<Vector4> tangents;
		Vector<Vector4> bitangents;
		Vector<RGBA> colors;
		Vector<Vector2> UV[FBX_IMPORT_MAX_UV_LAYERS];
		Vector<int> materials;

		Vector<int> smoothingGroups;
		Vector<FBXBlendShape> blendShapes;

		Vector<FBXBoneInfluence> boneInfluences;
		Vector<FBXBone> bones;

		MeshDataPtr meshData;
		Vector<SubMesh> subMeshes;

		Vector<FBXImportNode*> referencedBy;
	};

	/**
	 * @brief	Scene information used and modified during FBX import.
	 */
	struct FBXImportScene
	{
		FBXImportScene();
		~FBXImportScene();

		Vector<FBXImportMesh*> meshes;
		FBXImportNode* rootNode;

		UnorderedMap<FbxNode*, FBXImportNode*> nodeMap;
		UnorderedMap<FbxMesh*, UINT32> meshMap;

		// TODO - Store animation & bones
	};

}