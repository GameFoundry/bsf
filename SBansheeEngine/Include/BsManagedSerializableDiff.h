#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedSerializableDiff : public IReflectable
	{
	public:
		struct BS_SCR_BE_EXPORT Modification : public IReflectable
		{
			virtual ~Modification();

			/************************************************************************/
			/* 								RTTI		                     		*/
			/************************************************************************/
		public:
			friend class ModificationRTTI;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const;
		};

		struct BS_SCR_BE_EXPORT ModifiedField : public IReflectable
		{
			ModifiedField() { }
			ModifiedField(const ManagedSerializableTypeInfoPtr& parentType,
				const ManagedSerializableFieldInfoPtr& fieldType, const SPtr<Modification>& modification);

			ManagedSerializableTypeInfoPtr parentType;
			ManagedSerializableFieldInfoPtr fieldType;
			SPtr<Modification> modification;

			/************************************************************************/
			/* 								RTTI		                     		*/
			/************************************************************************/
		public:
			friend class ModifiedFieldRTTI;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const;
		};

		struct BS_SCR_BE_EXPORT ModifiedArrayEntry : public IReflectable
		{
			ModifiedArrayEntry() { }
			ModifiedArrayEntry(UINT32 idx, const SPtr<Modification>& modification);

			UINT32 idx;
			SPtr<Modification> modification;

			/************************************************************************/
			/* 								RTTI		                     		*/
			/************************************************************************/
		public:
			friend class ModifiedArrayEntryRTTI;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const;
		};

		struct BS_SCR_BE_EXPORT ModifiedDictionaryEntry : public IReflectable
		{
			ModifiedDictionaryEntry() { }
			ModifiedDictionaryEntry(const ManagedSerializableFieldDataPtr& key, const SPtr<Modification>& modification);

			ManagedSerializableFieldDataPtr key;
			SPtr<Modification> modification;

			/************************************************************************/
			/* 								RTTI		                     		*/
			/************************************************************************/
		public:
			friend class ModifiedArrayEntryRTTI;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const;
		};

		struct BS_SCR_BE_EXPORT ModifiedObject : Modification
		{
			static SPtr<ModifiedObject> create();

			Vector<ModifiedField> entries;

			/************************************************************************/
			/* 								RTTI		                     		*/
			/************************************************************************/
		public:
			friend class ModifiedObjectRTTI;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const;
		};

		struct BS_SCR_BE_EXPORT ModifiedArray : Modification
		{
			static SPtr<ModifiedArray> create();

			Vector<ModifiedArrayEntry> entries;
			Vector<UINT32> origSizes;
			Vector<UINT32> newSizes;

			/************************************************************************/
			/* 								RTTI		                     		*/
			/************************************************************************/
		public:
			friend class ModifiedArrayRTTI;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const;
		};

		struct BS_SCR_BE_EXPORT ModifiedDictionary : Modification
		{
			static SPtr<ModifiedDictionary> create();

			Vector<ModifiedDictionaryEntry> entries;
			Vector<ManagedSerializableFieldDataPtr> removed;

			/************************************************************************/
			/* 								RTTI		                     		*/
			/************************************************************************/
		public:
			friend class ModifiedDictionaryRTTI;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const;
		};

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
			virtual RTTITypeBase* getRTTI() const;
		};

	public:
		ManagedSerializableDiff();
		~ManagedSerializableDiff();

		static ManagedSerializableDiffPtr create(const ManagedSerializableObjectPtr& oldObj, const ManagedSerializableObjectPtr& newObj);
		void apply(const ManagedSerializableObjectPtr& obj);

	private:
		SPtr<ModifiedObject> generateDiff(const ManagedSerializableObjectPtr& oldObj, const ManagedSerializableObjectPtr& newObj);
		SPtr<Modification> generateDiff(const ManagedSerializableFieldDataPtr& oldData, const ManagedSerializableFieldDataPtr& newData,
			UINT32 fieldTypeId);
		ManagedSerializableFieldDataPtr applyDiff(const SPtr<ModifiedObject>& mod, const ManagedSerializableObjectPtr& obj);
		ManagedSerializableFieldDataPtr applyDiff(const SPtr<ModifiedArray>& mod, const ManagedSerializableArrayPtr& obj);
		ManagedSerializableFieldDataPtr applyDiff(const SPtr<ModifiedArray>& mod, const ManagedSerializableListPtr& obj);
		ManagedSerializableFieldDataPtr applyDiff(const SPtr<ModifiedDictionary>& mod, const ManagedSerializableDictionaryPtr& obj);
		ManagedSerializableFieldDataPtr applyDiff(const SPtr<Modification>& mod, const ManagedSerializableTypeInfoPtr& fieldType,
			const ManagedSerializableFieldDataPtr& origData);

		SPtr<ModifiedObject> mModificationRoot;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedSerializableDiffRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}