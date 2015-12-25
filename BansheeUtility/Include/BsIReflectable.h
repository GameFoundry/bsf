#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsAny.h"

namespace BansheeEngine
{
	/** @addtogroup RTTI
	 *  @{
	 */

	/**
	 * Interface implemented by classes that provide run time type information.
	 * 			
	 * @note	
	 * Any class implementing this interface must implement the getRTTI() method, as well as a static getRTTIStatic() 
	 * method, returning the same value as getRTTI(). Object returned by those methods is used for retrieving actual RTTI 
	 * data about the class.
	 */
	class BS_UTILITY_EXPORT IReflectable
	{
	public:
		virtual ~IReflectable() {}

		/**
		 * Returns an interface you can use to access class' Run Time Type Information.
		 *
		 * @note	
		 * You must derive your own version of RTTITypeBase, in which you may encapsulate all reflection specific operations. 
		 */
		virtual RTTITypeBase* getRTTI() const = 0;

		/** Returns all classes deriving directly from IReflectable. */
		static Vector<RTTITypeBase*>& getDerivedClasses()
		{
			static Vector<RTTITypeBase*> mRTTIDerivedClasses;
			return mRTTIDerivedClasses;
		}

		/** Returns true if current RTTI class is derived from @p base (Or if it is the same type as base). */
		bool isDerivedFrom(RTTITypeBase* base);

		/** Returns an unique type identifier of the class. */
		UINT32 getTypeId() const;

		/**
		 * Returns the type name of the class.
		 *
		 * @note	Name is not necessarily unique.
		 */
		const String& getTypeName() const;

		/** Creates an empty instance of a class from a type identifier. */
		static std::shared_ptr<IReflectable> createInstanceFromTypeId(UINT32 rttiTypeId);

		/**
		 * Called by each type deriving from IReflectable, on program load.
		 *
		 * @note	Internal method.
		 */
		static void _registerDerivedClass(RTTITypeBase* derivedClass);

		/**
		 * Returns class' RTTI type from type id.
		 *
		 * @note	Internal method.
		 */
		static RTTITypeBase* _getRTTIfromTypeId(UINT32 rttiTypeId);

		/**
		 * Checks if the provided type id is unique.
		 *
		 * @note	Internal method.
		 */
		static bool _isTypeIdDuplicate(UINT32 typeId);

	protected:
		Any mRTTIData; /**< Temporary per-instance data storage used during various RTTI operations.
					    Needed since there is one RTTI class instance per type and sometimes we need per-instance data. */
	};

	/** @} */
}