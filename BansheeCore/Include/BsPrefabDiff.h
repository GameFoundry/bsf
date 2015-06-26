#pragma once

#include "BsCorePrerequisites.h"
#include "BsIReflectable.h"
#include "BsGameObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains differences between two components of the same type.
	 *
	 * @see		PrefabDiff
	 */
	struct BS_CORE_EXPORT PrefabComponentDiff : public IReflectable
	{
		INT32 id;
		SPtr<SerializedObject> data;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class PrefabComponentDiffRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	Contains a set of prefab differences for a single scene object.
	 *
	 * @see		PrefabDiff
	 */
	struct BS_CORE_EXPORT PrefabObjectDiff : public IReflectable
	{
		INT32 id;
		String name;

		Vector<SPtr<PrefabComponentDiff>> componentDiffs;
		Vector<INT32> removedComponents;
		Vector<SPtr<SerializedObject>> addedComponents;

		Vector<SPtr<PrefabObjectDiff>> childDiffs;
		Vector<INT32> removedChildren;
		Vector<SPtr<SerializedObject>> addedChildren;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class PrefabObjectDiffRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	Contains modifications between an prefab and its instance. The modifications are a set of
	 *			added/removed children or components and per-field "diffs" of their components.
	 */
	class BS_CORE_EXPORT PrefabDiff : public IReflectable
	{
	public:
		/**
		 * @brief	Creates a new prefab diff by comparing the provided instanced scene object hierarchy
		 *			with the prefab scene object hierarchy.
		 */
		static SPtr<PrefabDiff> create(const HSceneObject& prefab, const HSceneObject& instance);

		/**
		 * @brief	Applies the internal prefab diff to the provided object. The object should have
		 *			similar hierarchy as the prefab the diff was created for, otherwise the results are
		 *			undefined.
		 */
		void apply(const HSceneObject& object);

	private:
		/**
		 * @brief	Recurses over every scene object in the prefab a generates differences between itself
		 *			and the instanced version.
		 *
		 * @see		create
		 */
		static SPtr<PrefabObjectDiff> generateDiff(const HSceneObject& prefab, const HSceneObject& instance);

		/**
		 * @brief	Recursively applies a per-object set of prefab differences to a specific object.
		 *
		 * @see		apply			
		 */
		static void applyDiff(const SPtr<PrefabObjectDiff>& diff, const HSceneObject& object);

		SPtr<PrefabObjectDiff> mRoot;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class PrefabDiffRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}