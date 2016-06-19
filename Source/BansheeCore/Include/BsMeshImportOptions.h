//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsImportOptions.h"

namespace BansheeEngine
{
	/** @addtogroup Importer
	 *  @{
	 */

	/** Controls what type of collision mesh should be imported during mesh import. */
	enum class CollisionMeshType
	{
		None, /**< No collision mesh will be imported. */
		Normal, /**< Normal triangle mesh will be imported. */
		Convex /**< A convex hull will be generated from the source mesh. */
	};

	/** Contains information about a piece of imported animation that will be used for generating its own AnimationClip. */
	struct AnimationSplitInfo : IReflectable
	{
		String name;
		UINT32 startFrame;
		UINT32 endFrame;
		bool isAdditive;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class AnimationSplitInfoRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/**
	 * Contains import options you may use to control how is a mesh imported from some external format into engine format.
	 */
	class BS_CORE_EXPORT MeshImportOptions : public ImportOptions
	{
	public:
		MeshImportOptions();

		/**	Sets whether the texture data is also stored in CPU memory. */
		void setCPUReadable(bool readable) { mCPUReadable = readable; }
		
		/**	Retrieves whether the texture data is also stored in CPU memory. */
		bool getCPUReadable() const { return mCPUReadable; }

		/**	Sets a value that controls should mesh normals be imported if available. */
		void setImportNormals(bool import) { mImportNormals = import; }

		/**	Retrieves a value that controls should mesh normals be imported if available. */
		bool getImportNormals() const { return mImportNormals; }

		/**	Sets a value that controls should mesh tangents/bitangents be imported if available. */
		void setImportTangents(bool import) { mImportTangents = import; }

		/**	Retrieves a value that controls should mesh tangent/bitangent be imported if available. */
		bool getImportTangents() const { return mImportTangents; }

		/**	Sets a value that controls should mesh blend shapes be imported	if available. */
		void setImportBlendShapes(bool import) { mImportBlendShapes = import; }

		/**	Retrieves a value that controls should mesh blend shapes be imported if available. */
		bool getImportBlendShapes() const { return mImportBlendShapes; }

		/**	
		 * Sets a value that controls should mesh skin data like bone weights, indices and bind poses be imported if 
		 * available.
		 */
		void setImportSkin(bool import) { mImportSkin = import; }

		/**
		 * Retrieves a value that controls should mesh skin data like bone weights, indices and bind poses be imported if 
		 * available.
		 */
		bool getImportSkin() const { return mImportSkin; }

		/** Sets a value that controls should animation clips be imported if available. */
		void setImportAnimation(bool import) { mImportAnimation = import; }

		/**	Retrieves a value that controls should animation clips be imported if available. */
		bool getImportAnimation() const { return mImportAnimation; }

		/**	Sets a value that will uniformly scale the imported mesh by the specified value. */
		void setImportScale(float import) { mImportScale = import; }

		/**	Retrieves a value that will uniformly scale the imported mesh by the specified value. */
		float getImportScale() const { return mImportScale; }

		/**	Sets a value that controls what type (if any) of collision mesh should be imported. */
		void setCollisionMeshType(CollisionMeshType type) { mCollisionMeshType = type; }

		/**	Retrieves a value that controls what type (if any) of collision mesh should be imported. */
		CollisionMeshType getCollisionMeshType() const { return mCollisionMeshType; }

		/** 
		 * Registers an animation split info that determines how will the source animation clip be split. If not splits
		 * are present the data will be imported as one clip, but if splits are present the data will be split according
		 * to the split infos.
		 */
		void addAnimationClipSplit(const AnimationSplitInfo& splitInfo) { mAnimationSplits.push_back(splitInfo); }

		/** Returns in how many pieces should the imported animation clip be split info. */
		UINT32 getNumAnimationClipSplits() const { return (UINT32)mAnimationSplits.size(); }

		/** Returns information about an animation split at the specified index. */
		const AnimationSplitInfo& getAnimationClipSplit(UINT32 idx) const { return mAnimationSplits[idx]; }

		/** Removes an animation split info at the specified index. */
		void removeAnimationClipSplit(UINT32 idx) { mAnimationSplits.erase(mAnimationSplits.begin() + idx); }

	private:
		bool mCPUReadable;
		bool mImportNormals;
		bool mImportTangents;
		bool mImportBlendShapes;
		bool mImportSkin;
		bool mImportAnimation;
		float mImportScale;
		CollisionMeshType mCollisionMeshType;
		Vector<AnimationSplitInfo> mAnimationSplits;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class MeshImportOptionsRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/** @} */
}