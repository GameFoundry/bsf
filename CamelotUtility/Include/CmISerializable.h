#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmIReflectable.h"
#include "CmSerializableType.h"

namespace CamelotEngine
{
	// TODO - Move these classes up to IReflectable, and remove ISerializable because it's not used
	class CM_UTILITY_EXPORT RTTIMetaTypeBase
	{
	public:
		//virtual String getRTTIName() = 0;
		//virtual UINT32 getRTTITypeId() = 0;
		//virtual RTTITypeBase* getRTTI() = 0;
		virtual IReflectable* newObject() = 0;
	};

	template <class T>
	class CM_UTILITY_EXPORT RTTIMetaType : public RTTIMetaTypeBase
	{
	public:
		//virtual String getRTTIName()
		//{ return T::getRTTIName(); } // TODO - Maybe add static asserts to add a better error message if method doesn't exist
		//virtual UINT32 getRTTITypeId() 
		//{ return T::getRTTITypeId(); }
		//virtual RTTITypeBase* getRTTI()
		//{ return T::getRTTI(); }
		virtual IReflectable* newObject()
		{ return T::newObject(); }
	};

	class CM_UTILITY_EXPORT ISerializable : public IReflectable
	{
	public:
		virtual RTTITypeBase* getRTTI() const { return getSerializable(); }
		virtual SerializableType* getSerializable() const = 0;

	protected:
		static vector<RTTIMetaTypeBase*>::type& getRTTIDerivedClasses()
		{
			static vector<RTTIMetaTypeBase*>::type mRTTIDerivedClasses;
			return mRTTIDerivedClasses;
		}

		static void registerRTTIDerivedClass(RTTIMetaTypeBase* derivedClass)
		{
			getRTTIDerivedClasses().push_back(derivedClass);
		}
	};
}