#pragma once

#include "BsPrerequisitesUtil.h"

namespace BansheeEngine
{
	/**
	 * @brief	Helper class that performs cloning of an object that implements RTTI.
	 */
	class BS_UTILITY_EXPORT BinaryCloner
	{
	public:

		/**
		 * @brief	Returns a copy of the provided object with identical data.
		 *
		 * @param[in]	object		Object to clone.
		 * @param[in]	shallow		If false then all referenced objects will be cloned
		 *							as well, otherwise the references to the original
		 *							objects will be kept.
		 */
		static SPtr<IReflectable> clone(IReflectable* object, bool shallow = false);

	private:
		struct ObjectReferenceData;

		/**
		 * @brief	Identifier representing a single field
		 *			or an array entry in an object.
		 */
		struct FieldId
		{
			RTTIField* field;
			INT32 arrayIdx;
		};

		/**
		 * @brief	A saved reference to an object with a field
		 *			identifier that owns it.
		 */
		struct ObjectReference
		{
			FieldId fieldId;
			SPtr<IReflectable> object;
		};

		/**
		 * @brief	Contains all object references in a portion of an
		 *			object belonging to a specific class (base and derived 
		 *			classes count as separate sub-objects).
		 */
		struct SubObjectReferenceData
		{
			RTTITypeBase* rtti;
			Vector<ObjectReference> references;
			Vector<ObjectReferenceData> children;
		};

		/**
		 * @brief	Contains all object references in an entire object, as well
		 *			as the identifier of the field owning this object.
		 */
		struct ObjectReferenceData
		{
			FieldId fieldId;
			Vector<SubObjectReferenceData> subObjectData;
		};

		/**
		 * @brief	Iterates over the provided object hierarchy and retrieves all object references
		 *			which are returned in "referenceData" output parameter, also in a hierarchical
		 *			format for easier parsing.
		 */
		static void gatherReferences(IReflectable* object, ObjectReferenceData& referenceData);

		/**
		 * @brief	Restores a set of references retrieved by "gatherReferences" and applies them to
		 *			a specific object. Type of the object must be the same as the type that was used
		 *			when calling "gatherReferences".
		 */
		static void restoreReferences(IReflectable* object, const ObjectReferenceData& referenceData);
	};
}