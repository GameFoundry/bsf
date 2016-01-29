//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsGameObject.h"
#include "BsResource.h"

namespace BansheeEngine
{
	/** @addtogroup Scene
	 *  @{
	 */

	/**
	 * Prefab is a saveable hierarchy of scene objects. In general it can serve as any grouping of scene objects 
	 * (e.g. a level) or be used as a form of a template instantiated and reused throughout the scene.
	 */
	class BS_CORE_EXPORT Prefab : public Resource
	{
	public:
		Prefab();
		~Prefab();

		/**
		 * Creates a new prefab from the provided scene object. If the scene object has an existing prefab link it will 
		 * be broken. After the prefab is created the scene object will be automatically linked to it.
		 */
		static HPrefab create(const HSceneObject& sceneObject);

		/**
		 * Instantiates a prefab by creating an instance of the prefab's scene object hierarchy. The returned hierarchy 
		 * will be parented to world root by default.
		 *			
		 * @return	Instantiated clone of the prefab's scene object hierarchy.
		 */
		HSceneObject instantiate();

		/**
		 * Replaces the contents of this prefab with new contents from the provided object. Object will be automatically
		 * linked to this prefab, and its previous prefab link (if any) will be broken.
		 */
		void update(const HSceneObject& sceneObject);

		/**
		 * Returns a hash value that can be used for determining if a prefab changed by comparing it to a previously saved
		 * hash.
		 */
		UINT32 getHash() const { return mHash; }

	public: // ***** INTERNAL ******
		/** @cond INTERNAL */

		/** Updates any prefab child instances by loading their prefabs and making sure they are up to date. */
		void _updateChildInstances();

		/**
		 * Returns a reference to the internal prefab hierarchy. Returned hierarchy is not instantiated and cannot be 
		 * interacted with in a manner you would with normal scene objects.
		 */
		HSceneObject _getRoot() const { return mRoot; }

		/**
		 * Creates the clone of the prefab's current hierarchy but doesn't instantiate it.
		 *			
		 * @return	Clone of the prefab's scene object hierarchy.
		 */
		HSceneObject _clone();

		/** @endcond */

	private:
		/**	Initializes the internal prefab hierarchy. Must be called druing creation. */
		void initialize(const HSceneObject& sceneObject);

		/**	Creates an empty and uninitialized prefab. */
		static PrefabPtr createEmpty();

		HSceneObject mRoot;
		UINT32 mHash;
		String mUUID;
		UINT32 mNextLinkId;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class PrefabRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/** @} */
}