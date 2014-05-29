#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsAny.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interface implemented by classes that provide run time type information.
	 * 			
	 * @note	Any class implementing this interface must implement the "getRTTI" method, as well as
	 * 			a static "getRTTIStatic" method, returning the same value as "getRTTI". Object returned by those
	 * 			methods is used for retrieving actual RTTI data about the class.
	 */
	class BS_UTILITY_EXPORT IReflectable
	{
	public:
		virtual ~IReflectable() {}

		/**
		 * @brief	Returns an interface you can use to access class' Run Time Type Information.
		 *
		 * @note	You must derive your own version of RTTITypeBase, in which you
		 * 			may encapsulate all reflection specific operations. 
		 */
		virtual RTTITypeBase* getRTTI() const = 0;

		/**
		 * @brief	Returns all classes deriving directly from IReflectable.
		 */
		static Vector<RTTITypeBase*>& getDerivedClasses()
		{
			static Vector<RTTITypeBase*> mRTTIDerivedClasses;
			return mRTTIDerivedClasses;
		}

		/**
		 * @brief	Returns true if current RTTI class is derived from "base".
		 * 			(Or if it is the same type as base)
		 */
		bool isDerivedFrom(RTTITypeBase* base);

		/**
		 * @brief	Returns an unique type identifier of the class.
		 */
		UINT32 getTypeId() const;

		/**
		 * @brief	Returns the type name of the class.
		 *
		 * @note	Name is not necessarily unique.
		 */
		const String& getTypeName() const;

		/**
		 * @brief	Creates an empty instance of a class from a type identifier.
		 */
		static std::shared_ptr<IReflectable> createInstanceFromTypeId(UINT32 rttiTypeId);

		/**
		 * @brief	Internal method. Called by each type deriving from IReflectable,
		 * 			on program load.
		 */
		static void _registerDerivedClass(RTTITypeBase* derivedClass);

		/**
		* @brief	Internal method. Returns class' RTTI type from type id.
		*/
		static RTTITypeBase* _getRTTIfromTypeId(UINT32 rttiTypeId);

		/**
		 * @brief	Internal method. Checks if the provided type id is unique.
		 */
		static bool _isTypeIdDuplicate(UINT32 typeId);

	protected:
		Any mRTTIData; // Temporary per-instance data storage used during various RTTI operations.
					   // Needed since there is one RTTI class instance per type and sometimes we need per-instance data.
	};
}