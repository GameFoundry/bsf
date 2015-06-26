#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsPrefabDiff.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT PrefabComponentDiffRTTI : public RTTIType < PrefabComponentDiff, IReflectable, PrefabComponentDiffRTTI >
	{
	private:
		BS_PLAIN_MEMBER(id)
		BS_REFLPTR_MEMBER(data);
	public:
		PrefabComponentDiffRTTI()
		{
			BS_ADD_PLAIN_FIELD(id, 0);
			BS_ADD_REFLPTR_FIELD(data, 1);
		}

		virtual const String& getRTTIName() override
		{
			static String name = "PrefabComponentDiff";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_PrefabComponentDiff;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr<PrefabComponentDiff>();
		}
	};

	class BS_CORE_EXPORT PrefabObjectDiffRTTI : public RTTIType < PrefabObjectDiff, IReflectable, PrefabObjectDiffRTTI >
	{
	private:
		BS_PLAIN_MEMBER(id)
		BS_PLAIN_MEMBER(name)

		BS_REFLPTR_MEMBER_VEC(componentDiffs)
		BS_PLAIN_MEMBER_VEC(removedComponents)
		BS_REFLPTR_MEMBER_VEC(addedComponents)

		BS_REFLPTR_MEMBER_VEC(childDiffs)
		BS_PLAIN_MEMBER_VEC(removedChildren)
		BS_REFLPTR_MEMBER_VEC(addedChildren)
	public:
		PrefabObjectDiffRTTI()
		{
			BS_ADD_PLAIN_FIELD(id, 0);
			BS_ADD_PLAIN_FIELD(name, 1);

			BS_ADD_REFLPTR_FIELD_ARR(componentDiffs, 2);
			BS_ADD_PLAIN_FIELD_ARR(removedComponents, 3);
			BS_ADD_REFLPTR_FIELD_ARR(addedComponents, 4);

			BS_ADD_REFLPTR_FIELD_ARR(childDiffs, 5);
			BS_ADD_PLAIN_FIELD_ARR(removedChildren, 6);
			BS_ADD_REFLPTR_FIELD_ARR(addedChildren, 7);
		}

		virtual const String& getRTTIName() override
		{
			static String name = "PrefabObjectDiff";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_PrefabObjectDiff;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr<PrefabObjectDiff>();
		}
	};

	class BS_CORE_EXPORT PrefabDiffRTTI : public RTTIType < PrefabDiff, IReflectable, PrefabDiffRTTI >
	{
	private:
		BS_REFLPTR_MEMBER(mRoot);
	public:
		PrefabDiffRTTI()
		{
			BS_ADD_REFLPTR_FIELD(mRoot, 0);
		}

		virtual const String& getRTTIName() override
		{
			static String name = "PrefabDiff";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_PrefabDiff;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr<PrefabDiff>();
		}
	};
}