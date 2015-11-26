#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles creation and applying of managed diffs. A diff contains differences
	 *			between two objects of identical types. If the initial state of an object
	 *			is known the recorded differences can be saved and applied to the original
	 *			state to restore the modified object.
	 *
	 *			Differences are recorded per primitive field in an object. 
	 *			Complex objects are recursed. Special handling is implemented
	 *			to properly generate diffs for arrays, lists and dictionaries.
	 *
	 *			All primitive types supported by managed serialization are supported.
	 *			(see ScriptPrimitiveType)
	 */
	class BS_SCR_BE_EXPORT ManagedSerializableDiff : public IReflectable
	{
	public:
		/**
		 * @brief	A base class for all modifications recorded in a diff.
		 */
		struct BS_SCR_BE_EXPORT Modification : public IReflectable
		{
			virtual ~Modification();

			/************************************************************************/
			/* 								RTTI		                     		*/
			/************************************************************************/
		public:
			friend class ModificationRTTI;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const override;
		};

		/**
		 * @brief	Contains a modification of a specific field in an object along
		 *			with information about the field and its parent object.
		 */
		struct BS_SCR_BE_EXPORT ModifiedField : public IReflectable
		{
			ModifiedField() { }
			ModifiedField(const ManagedSerializableTypeInfoPtr& parentType,
				const ManagedSerializableFieldInfoPtr& fieldType, const SPtr<Modification>& modification);

			ManagedSerializableTypeInfoPtr parentType; /**< Type of the parent object the field belongs to. */
			ManagedSerializableFieldInfoPtr fieldType; /**< Data type of the field. */
			SPtr<Modification> modification; /**< Recorded modification(s) on the field. */

			/************************************************************************/
			/* 								RTTI		                     		*/
			/************************************************************************/
		public:
			friend class ModifiedFieldRTTI;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const override;
		};

		/**
		 * @brief	Represents a single modified array or list entry.
		 */
		struct BS_SCR_BE_EXPORT ModifiedArrayEntry : public IReflectable
		{
			ModifiedArrayEntry() { }
			ModifiedArrayEntry(UINT32 idx, const SPtr<Modification>& modification);

			UINT32 idx; /**< Index of the array/list entry that is modified. */
			SPtr<Modification> modification; /**< Recorded modification(s) on the entry. */

			/************************************************************************/
			/* 								RTTI		                     		*/
			/************************************************************************/
		public:
			friend class ModifiedArrayEntryRTTI;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const override;
		};

		/**
		 * @brief	Represents a single modified dictionary entry.
		 */
		struct BS_SCR_BE_EXPORT ModifiedDictionaryEntry : public IReflectable
		{
			ModifiedDictionaryEntry() { }
			ModifiedDictionaryEntry(const ManagedSerializableFieldDataPtr& key, const SPtr<Modification>& modification);

			ManagedSerializableFieldDataPtr key; /**< Serialized value of the key for the modified entry. */
			SPtr<Modification> modification; /**< Recorded modification(s) on the dictionary entry value. */

			/************************************************************************/
			/* 								RTTI		                     		*/
			/************************************************************************/
		public:
			friend class ModifiedArrayEntryRTTI;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const override;
		};

		/**
		 * @brief	Contains data about all modifications in a single complex object.
		 *			(aside from arrays, list, dictionaries which are handled specially).
		 */
		struct BS_SCR_BE_EXPORT ModifiedObject : Modification
		{
			static SPtr<ModifiedObject> create();

			Vector<ModifiedField> entries; /**< A list of entries containing each modified field in the object. */

			/************************************************************************/
			/* 								RTTI		                     		*/
			/************************************************************************/
		public:
			friend class ModifiedObjectRTTI;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const override;
		};

		/**
		 * @brief	Contains data about all modifications in an array or a list.
		 */
		struct BS_SCR_BE_EXPORT ModifiedArray : Modification
		{
			static SPtr<ModifiedArray> create();

			Vector<ModifiedArrayEntry> entries; /**< A list of all modified array/list entries along with their indices. */
			Vector<UINT32> origSizes; /**< Original size of the array/list (one size per dimension). */
			Vector<UINT32> newSizes; /**< New size of the array/list (one size per dimension). */

			/************************************************************************/
			/* 								RTTI		                     		*/
			/************************************************************************/
		public:
			friend class ModifiedArrayRTTI;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const override;
		};

		/**
		 * @brief	Contains data about all modifications in a dictionary.
		 */
		struct BS_SCR_BE_EXPORT ModifiedDictionary : Modification
		{
			static SPtr<ModifiedDictionary> create();

			Vector<ModifiedDictionaryEntry> entries; /**< A list of modified entries in the dictionary. */
			Vector<ManagedSerializableFieldDataPtr> removed; /**< A list of keys for entries that were removed from the dictionary. */

			/************************************************************************/
			/* 								RTTI		                     		*/
			/************************************************************************/
		public:
			friend class ModifiedDictionaryRTTI;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const override;
		};

		/**
		 * @brief	Contains data about modification of a primitive field.
		 *			(i.e. fields new value)
		 */
		struct BS_SCR_BE_EXPORT ModifiedEntry : Modification
		{
			ModifiedEntry() { }
			ModifiedEntry(const ManagedSerializableFieldDataPtr& value);

			static SPtr<ModifiedEntry> create(const ManagedSerializableFieldDataPtr& value);

			ManagedSerializableFieldDataPtr value;

			/************************************************************************/
			/* 								RTTI		                     		*/
			/************************************************************************/
		public:
			friend class ModifiedEntryRTTI;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const override;
		};

	public:
		ManagedSerializableDiff();
		~ManagedSerializableDiff();

		/**
		 * @brief	Generates a new managed diff object by comparing two objects of the same type. Callers must
		 *			ensure both objects are not null and of identical types.
		 *
		 * @param	oldObj	Original object. This is the object you can apply the diff to to convert it to \p newObj.
		 * @param	newObj	New modified object. Any values in this object that differ from the original object will be
		 *					recorded in the diff.
		 *
		 * @return	Returns null if objects are identical.
		 */
		static ManagedSerializableDiffPtr create(const ManagedSerializableObjectPtr& oldObj, const ManagedSerializableObjectPtr& newObj);

		/**
		 * @brief	Applies the diff data stored in this object to the specified object, modifying all
		 *			fields in the object to correspond to the stored diff data.
		 */
		void apply(const ManagedSerializableObjectPtr& obj);

	private:
		/**
		 * @brief	Recursively generates a diff between all fields of the specified objects. Returns null if objects are identical.
		 */
		SPtr<ModifiedObject> generateDiff(const ManagedSerializableObjectPtr& oldObj, const ManagedSerializableObjectPtr& newObj);

		/**
		 * @brief	Generates a diff between two fields. Fields can be of any type and the system will generate the diff appropriately.
		 *			Diff is generated recursively on all complex objects as well. Returns null if fields contain identical data.
		 */
		SPtr<Modification> generateDiff(const ManagedSerializableFieldDataPtr& oldData, const ManagedSerializableFieldDataPtr& newData,
			UINT32 fieldTypeId);

		/**
		 * @brief	Applies an object modification to a managed object. Modifications are applied recursively.
		 *
		 * @param	mod Object modification to apply.
		 * @param	obj	Object to apply the modification to.
		 *
		 * @return	New field data in the case modification needed the object to be re-created instead of just modified.
		 */
		ManagedSerializableFieldDataPtr applyDiff(const SPtr<ModifiedObject>& mod, const ManagedSerializableObjectPtr& obj);

		/**
		 * @brief	Applies an array modification to a managed array. Modifications are applied recursively.
		 *
		 * @param	mod Array modification to apply.
		 * @param	obj	Array to apply the modification to.
		 *
		 * @return	New field data in the case modification needed the array to be re-created instead of just modified.
		 */
		ManagedSerializableFieldDataPtr applyDiff(const SPtr<ModifiedArray>& mod, const ManagedSerializableArrayPtr& obj);

		/**
		 * @brief	Applies an list modification to a managed list. Modifications are applied recursively.
		 *
		 * @param	mod List modification to apply.
		 * @param	obj	List to apply the modification to.
		 *
		 * @return	New field data in the case modification needed the list to be re-created instead of just modified.
		 */
		ManagedSerializableFieldDataPtr applyDiff(const SPtr<ModifiedArray>& mod, const ManagedSerializableListPtr& obj);

		/**
		 * @brief	Applies an dictionary modification to a managed dictionary. Modifications are applied recursively.
		 *
		 * @param	mod Dictionary modification to apply.
		 * @param	obj	Dictionary to apply the modification to.
		 *
		 * @return	New field data in the case modification needed the dictionary to be re-created instead of just modified.
		 */
		ManagedSerializableFieldDataPtr applyDiff(const SPtr<ModifiedDictionary>& mod, const ManagedSerializableDictionaryPtr& obj);

		/**
		 * @brief	Applies a modification to a single field. Field type is determined and the modification is applied
		 *			to the specific field type as needed. Modifications are applied recursively.
		 *
		 * @param	mod			Modification to apply.
		 * @param	fieldType	Type of the field we're applying the modification to.
		 * @param	origData	Original data of the field.
		 *
		 * @return	New field data in the case modification needed the field data to be re-created instead of just modified.
		 */
		ManagedSerializableFieldDataPtr applyDiff(const SPtr<Modification>& mod, const ManagedSerializableTypeInfoPtr& fieldType,
			const ManagedSerializableFieldDataPtr& origData);

		SPtr<ModifiedObject> mModificationRoot;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableDiffRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}