#pragma once

#include <string>
#include <algorithm>
#include <unordered_map>

#include <boost/preprocessor/punctuation/comma.hpp>

#include "CmPrerequisitesUtil.h"
#include "CmManagedDataBlock.h"
#include "CmRTTIField.h"
#include "CmRTTIPlainField.h"
#include "CmRTTIReflectableField.h"
#include "CmRTTIReflectablePtrField.h"
#include "CmRTTIManagedDataBlockField.h"
#include "CmIReflectable.h"

namespace CamelotFramework
{
#define CM_SETGET_MEMBER(name, type, parentType)								\
	type##& get##name(parentType##* obj) { return obj->##name; }				\
	void Set##name(parentType##* obj, type##& val) { obj->##name = val; } 

#define CM_ADD_PLAINFIELD(name, id, parentType) \
	addPlainField(#name, id##, &##parentType##::get##name, &##parentType##::Set##name);

	/**
	 * @brief	Provides an interface for accessing fields of a certain class.
	 * 			Data can be easily accessed by getter and setter methods.
	 * 			
	 *			Supported data types:
	 *			 - Plain types - All types defined in CmRTTIField.h, mostly native types and POD (plain old data) structs. Data is parsed byte by byte.  
	 *			                 No pointers to plain types are supported. Data is passed around by value.
	 *			 - Reflectable types - Any class deriving from IReflectable. Data is parsed based on fields in its ReflectionInterface. Can be pointer or value type.
	 *			 - Arrays of both plain and reflectable types are supported
	 *			 - Data blocks - A managed or unmanaged block of data. See CmManagedDataBlock.h
	 */
	class CM_UTILITY_EXPORT RTTITypeBase
	{
	public:
		RTTITypeBase();
		virtual ~RTTITypeBase();

		virtual Vector<RTTITypeBase*>::type& getDerivedClasses() = 0;
		virtual RTTITypeBase* getBaseClass() = 0;
		virtual void registerDerivedClass(RTTITypeBase* derivedClass) = 0;
		virtual std::shared_ptr<IReflectable> newRTTIObject() = 0;
		virtual const String& getRTTIName() = 0;
		virtual UINT32 getRTTIId() = 0;

		virtual void onSerializationStarted(IReflectable* obj) {}
		virtual void onSerializationEnded(IReflectable* obj) {}
		virtual void onDeserializationStarted(IReflectable* obj) {}
		virtual void onDeserializationEnded(IReflectable* obj) {}

		template <class ObjectType, class DataType>
		void setPlainValue(ObjectType* object, const String& name, DataType& value)
		{
			RTTIField* genericField = findField(name);
			genericField->checkIsPlain(false);

			RTTIPlainFieldBase* field = static_cast<RTTIPlainFieldBase*>(genericField);

			UINT32 typeSize = 0;
			if(RTTIPlainType<DataType>::hasDynamicSize)
				typeSize = RTTIPlainType<DataType>::getDynamicSize(value);
			else
				typeSize = sizeof(DataType);

			UINT8* tempBuffer = (UINT8*)stackAlloc(typeSize);
			RTTIPlainType<DataType>::toMemory(value, (char*)tempBuffer);
			
			field->fromBuffer(object, tempBuffer);

			stackDeallocLast(tempBuffer);
		}

		template <class ObjectType, class DataType>
		void setPlainArrayValue(ObjectType* object, const String& name, UINT32 index, DataType& value)
		{
			RTTIField* genericField = findField(name);
			genericField->checkIsPlain(true);

			RTTIPlainFieldBase* field = static_cast<RTTIPlainFieldBase*>(genericField);

			UINT32 typeSize = 0;
			if(RTTIPlainType<DataType>::hasDynamicSize)
				typeSize = RTTIPlainType<DataType>::getDynamicSize(value);
			else
				typeSize = sizeof(DataType);

			UINT8* tempBuffer = (UINT8*)stackAlloc(typeSize);
			RTTIPlainType<DataType>::toMemory(value, (char*)tempBuffer);

			field->arrayElemFromBuffer(object, index, tempBuffer);

			stackDeallocLast(tempBuffer);
		}

		template <class ObjectType, class DataType>
		void setReflectableValue(ObjectType* object, const String& name, DataType& value)
		{
			static_assert((std::is_base_of<CamelotFramework::IReflectable, DataType>::value), 
				"Invalid data type for complex field. It needs to derive from CamelotFramework::IReflectable.");

			RTTIField* genericField = findField(name);
			genericField->checkIsComplex(false);

			RTTIReflectableFieldBase* field = static_cast<RTTIReflectableFieldBase*>(genericField);
			field->setValue(object, value);
		}

		template <class ObjectType, class DataType>
		void setReflectableArrayValue(ObjectType* object, const String& name, UINT32 index, DataType& value)
		{
			static_assert((std::is_base_of<CamelotFramework::IReflectable, DataType>::value), 
				"Invalid data type for complex field. It needs to derive from CamelotFramework::IReflectable.");

			RTTIField* genericField = findField(name);
			genericField->checkIsComplex(true);

			RTTIReflectableFieldBase* field = static_cast<RTTIReflectableFieldBase*>(genericField);
			field->setArrayValue(object, index, value);
		}

		template <class ObjectType>
		void setDataBlockValue(ObjectType* object, const String& name, ManagedDataBlock value)
		{
			RTTIField* genericField = findField(name);
			genericField->checkIsDataBlock();

			RTTIManagedDataBlockFieldBase* field = static_cast<RTTIManagedDataBlockFieldBase*>(genericField);
			field->setValue(object, value);
		}


		template <class ObjectType, class DataType>
		void setReflectablePtrValue(ObjectType* object, const String& name, std::shared_ptr<DataType> value)
		{
			static_assert((std::is_base_of<CamelotFramework::IReflectable, DataType>::value), 
				"Invalid data type for complex field. It needs to derive from CamelotFramework::IReflectable.");

			RTTIField* genericField = findField(name);
			genericField->checkIsComplexPtr(false);

			RTTIReflectablePtrFieldBase* field = static_cast<RTTIReflectablePtrFieldBase*>(genericField);
			field->setValue(object, value);
		}

		template <class ObjectType, class DataType>
		void setReflectablePtrArrayValue(ObjectType* object, const String& name, UINT32 index, std::shared_ptr<DataType> value)
		{
			static_assert((std::is_base_of<CamelotFramework::IReflectable, DataType>::value), 
				"Invalid data type for complex field. It needs to derive from CamelotFramework::IReflectable.");

			RTTIField* genericField = findField(name);
			genericField->checkIsComplexPtr(true);

			RTTIReflectablePtrFieldBase* field = static_cast<RTTIReflectablePtrFieldBase*>(genericField);
			field->setArrayValue(object, index, value);
		}

		template <class ObjectType, class DataType>
		void getPlainValue(ObjectType* object, const String& name, DataType& value)
		{
			RTTIField* genericField = findField(name);
			genericField->checkIsPlain(false);

			RTTIPlainFieldBase* field = static_cast<RTTIPlainFieldBase*>(genericField);

			UINT32 typeSize = 0;
			if(field->hasDynamicSize())
				typeSize = field->getDynamicSize(object);
			else
				typeSize = field->getTypeSize();

			UINT8* tempBuffer = (UINT8*)stackAlloc(typeSize);

			field->toBuffer(object, tempBuffer);
			RTTIPlainType<DataType>::fromMemory(value, (char*)tempBuffer);

			stackDeallocLast(tempBuffer);
		}

		template <class ObjectType, class DataType>
		void getPlainArrayValue(ObjectType* object, const String& name, UINT32 index, DataType& value)
		{
			RTTIField* genericField = findField(name);
			genericField->checkIsPlain(true);

			RTTIPlainFieldBase* field = static_cast<RTTIPlainFieldBase*>(genericField);

			UINT32 typeSize = 0;
			if(field->hasDynamicSize())
				typeSize = field->getArrayElemDynamicSize(object, arrIdx);
			else
				typeSize = field->getTypeSize();

			UINT8* tempBuffer = (UINT8*)stackAlloc(typeSize);

			field->arrayElemToBuffer(object, index, tempBuffer);
			RTTIPlainType<DataType>::fromMemory(value, (char*)tempBuffer);

			stackDeallocLast(tempBuffer);
		}	

		template <class ObjectType>
		IReflectable& getReflectableValue(ObjectType* object, const String& name)
		{
			RTTIField* genericField = findField(name);
			genericField->checkIsComplex(false);

			RTTIReflectableFieldBase* field = static_cast<RTTIReflectableFieldBase*>(genericField);
			return field->getValue(object);
		}

		template <class ObjectType>
		IReflectable& getReflectableArrayValue(ObjectType* object, const String& name, UINT32 index)
		{
			RTTIField* genericField = findField(name);
			genericField->checkIsComplex(true);

			RTTIReflectableFieldBase* field = static_cast<RTTIReflectableFieldBase*>(genericField);
			return field->getArrayValue(object, index);
		}

		template <class ObjectType>
		ManagedDataBlock getDataBlockValue(ObjectType* object, const String& name)
		{
			RTTIField* genericField = findField(name);
			genericField->checkIsDataBlock();

			RTTIManagedDataBlockFieldBase* field = static_cast<RTTIManagedDataBlockFieldBase*>(genericField);
			return field->getValue(object);
		}

		template <class ObjectType>
		std::shared_ptr<IReflectable> getReflectablePtrValue(ObjectType* object, const String& name)
		{
			RTTIField* genericField = findField(name);
			genericField->checkIsComplexPtr(false);

			RTTIReflectablePtrFieldBase* field = static_cast<RTTIReflectablePtrFieldBase*>(genericField);
			return field->getValue(object);
		}

		template <class ObjectType>
		std::shared_ptr<IReflectable> getReflectablePtrArrayValue(ObjectType* object, const String& name, UINT32 index)
		{
			RTTIField* genericField = findField(name);
			genericField->checkIsComplexPtr(true);

			RTTIReflectablePtrFieldBase* field = static_cast<RTTIReflectablePtrFieldBase*>(genericField);
			return field->getArrayValue(object, index);
		}

		template <class ObjectType>
		UINT32 getArraySize(ObjectType* object, const String& name)
		{
			RTTIField* field = findField(name);
			return field->getArraySize(object);
		}

		template <class ObjectType>
		void setArraySize(ObjectType* object, const String& name, UINT32 size)
		{
			RTTIField* field = findField(name);
			field->setArraySize(object, size);
		}	

		UINT32 getNumFields() { return (UINT32)mFields.size(); }
		RTTIField* getField(UINT32 idx) { return mFields.at(idx); }

		/**
		 * @brief	Tries to find a field with the specified name. Throws an exception if it can't.
		 *
		 * @param	name	The name of the field.
		 */
		RTTIField* findField(const String& name);

		/**
		 * @brief	Tries to find a field with the specified unique ID. Doesn't throw an exception
		 * 			if it can't find the field (Unlike findField(name)). 
		 *
		 * @param	uniqueFieldId	Unique identifier for the field.
		 *
		 * @return	nullptr if it can't find the field.
		 */
		RTTIField* findField(int uniqueFieldId);

	protected:
		/**
		 * @brief	Tries to add a new field to the fields array, and throws an exception
		 * 			if a field with the same name or id already exists.
		 *
		 * @param [in]	field	Field, must be non-null.
		 */
		void addNewField(RTTIField* field);

		/**
		 * @brief	Checks if the templated DataType has any references back to us, that aren't weak.
		 * 			
		 * @note	This method assumes this class holds a non-weak reference to DataType.
		 * 			DataType must derive from IReflectable and implement getRTTIStatic method.
		 */
		template<class DataType>
		void checkForCircularReferences()
		{
			RTTITypeBase* type = DataType::getRTTIStatic();

			for(UINT32 i = 0; i < type->getNumFields(); i++)
			{
				RTTIField* field = type->getField(i);

				if(!field->isReflectablePtrType())
					continue;

				RTTIReflectablePtrFieldBase* reflectablePtrField = static_cast<RTTIReflectablePtrFieldBase*>(field);

				if(reflectablePtrField->getRTTIId() == getRTTIId() && ((reflectablePtrField->getFlags() & RTTI_Flag_WeakRef) == 0))
				{
					throwCircularRefException(getRTTIName(), reflectablePtrField->getRTTIName());
				}
			}
		}

		/**
		 * @brief	Throws an exception warning the user that a circular reference was found. 
		 *
		 * @note Only a separate function so I don't need to include CmException header.
		 */
		void throwCircularRefException(const String& myType, const String& otherType) const;

	private:
		Vector<RTTIField*>::type mFields;
	};

	/**
	 * @brief	Used for initializing a certain type as soon as the program is loaded.
	 */
	template<typename Type, typename BaseType>
	struct InitRTTIOnStart
	{
	public:
		InitRTTIOnStart()
		{
			BaseType::getRTTIStatic()->registerDerivedClass(Type::getRTTIStatic());
		}

		void makeSureIAmInstantiated() { }
	};

	/**
	 * @brief	Specialization for root class of RTTI hierarchy - IReflectable
	 */
	template<typename Type>
	struct InitRTTIOnStart<Type, IReflectable>
	{
	public:
		InitRTTIOnStart()
		{
			IReflectable::registerDerivedClass(Type::getRTTIStatic());
		}

		void makeSureIAmInstantiated() { }
	};

	/**
	 * @brief	Template that returns RTTI type of the specified type, unless the specified
	 * 			type is IReflectable in which case it returns a null.
	 */
	template<typename Type>
	struct GetRTTIType
	{
		RTTITypeBase* operator()() { return Type::getRTTIStatic(); }
	};

	template<>
	struct GetRTTIType<IReflectable>
	{
		RTTITypeBase* operator()() { return nullptr; }
	};

	/**
	 * @brief	Allows you to provide a run-time type information for a specific class, along with 
	 * 			support for serialization/deserialization.
	 * 			
	 * 			Derive from this class and return the derived class from IReflectable::getRTTI. 
	 * 			This way you can separate serialization logic from
	 * 			the actual class you're serializing.
	 */
	template <typename Type, typename BaseType, typename MyRTTIType>
	class RTTIType : public RTTITypeBase
	{
	protected:
		/************************************************************************/
		/* 						RTTI CLASS META DATA							*/
		/************************************************************************/

		static InitRTTIOnStart<Type, BaseType> initOnStart;

	public:
		RTTIType() 
		{
			// Compiler will only generate code for stuff that is directly used, including static data members,
			// so we fool it here like we're using the class directly. Otherwise compiler won't generate the code for the member
			// and our type won't get initialized on start (Actual behavior is a bit more random)
			initOnStart.makeSureIAmInstantiated();
		}
		virtual ~RTTIType() {}

		static MyRTTIType* instance()
		{
			static MyRTTIType inst;
			return &inst;
		}

		virtual Vector<RTTITypeBase*>::type& getDerivedClasses()
		{
			static Vector<RTTITypeBase*>::type mRTTIDerivedClasses;
			return mRTTIDerivedClasses;
		}

		virtual RTTITypeBase* getBaseClass()
		{
			return GetRTTIType<BaseType>()();
		}

		virtual void registerDerivedClass(RTTITypeBase* derivedClass)
		{
			if(IReflectable::isTypeIdDuplicate(derivedClass->getRTTIId()))
			{
				CM_EXCEPT(InternalErrorException, "RTTI type \"" + derivedClass->getRTTIName() + 
					"\" has a duplicate ID: " + toString(derivedClass->getRTTIId()));
			}

			getDerivedClasses().push_back(derivedClass);
		}

		/************************************************************************/
		/* 			FIELDS OPERATING DIRECTLY ON SERIALIZABLE OBJECT            */
		/************************************************************************/
		template<class ObjectType, class DataType>
		void addPlainField(const String& name, UINT32 uniqueId, DataType& (ObjectType::*getter)(), 
			void (ObjectType::*setter)(DataType&) = nullptr, UINT64 flags = 0)
		{
			addPlainField<ObjectType, DataType>(name, uniqueId, 
				std::function<DataType&(ObjectType*)>(getter), 
				std::function<void(ObjectType*, DataType&)>(setter), flags);
		}

		template<class ObjectType, class DataType>
		void addReflectableField(const String& name, UINT32 uniqueId, DataType& (ObjectType::*getter)(), 
			void (ObjectType::*setter)(DataType&) = nullptr, UINT64 flags = 0)
		{
			addReflectableField<ObjectType, DataType>(name, uniqueId, 
				std::function<DataType&(ObjectType*)>(getter), 
				std::function<void(ObjectType*, DataType&)>(setter), flags);
		}

		template<class ObjectType, class DataType>
		void addReflectablePtrField(const String& name, UINT32 uniqueId, std::shared_ptr<DataType> (ObjectType::*getter)(), 
			void (ObjectType::*setter)(std::shared_ptr<DataType>) = nullptr, UINT64 flags = 0)
		{
			addReflectablePtrField<ObjectType, DataType>(name, uniqueId, 
				std::function<std::shared_ptr<DataType>(ObjectType*)>(getter), 
				std::function<void(ObjectType*, std::shared_ptr<DataType>)>(setter), flags);
		}

		template<class ObjectType, class DataType>
		void addPlainArrayField(const String& name, UINT32 uniqueId, DataType& (ObjectType::*getter)(UINT32), UINT32 (ObjectType::*getSize)(), 
			void (ObjectType::*setter)(UINT32, DataType&) = nullptr, void(ObjectType::*setSize)(UINT32) = nullptr, UINT64 flags = 0)
		{
			addPlainArrayField<ObjectType, DataType>(name, uniqueId, 
				std::function<DataType&(ObjectType*, UINT32)>(getter), 
				std::function<UINT32(ObjectType*)>(getSize), 
				std::function<void(ObjectType*, UINT32, DataType&)>(setter), 
				std::function<void(ObjectType*, UINT32)>(setSize), flags);
		}	

		template<class ObjectType, class DataType>
		void addReflectableArrayField(const String& name, UINT32 uniqueId, DataType& (ObjectType::*getter)(UINT32), UINT32 (ObjectType::*getSize)(), 
			void (ObjectType::*setter)(UINT32, DataType&) = nullptr, void(ObjectType::*setSize)(UINT32) = nullptr, UINT64 flags = 0)
		{
			addReflectableArrayField<ObjectType, DataType>(name, uniqueId, 
				std::function<DataType&(ObjectType*, UINT32)>(getter), 
				std::function<UINT32(ObjectType*)>(getSize), 
				std::function<void(ObjectType*, UINT32, DataType&)>(setter), 
				std::function<void(ObjectType*, UINT32)>(setSize), flags);
		}

		template<class ObjectType, class DataType>
		void addReflectablePtrArrayField(const String& name, UINT32 uniqueId, std::shared_ptr<DataType> (ObjectType::*getter)(UINT32), UINT32 (ObjectType::*getSize)(), 
			void (ObjectType::*setter)(UINT32, std::shared_ptr<DataType>) = nullptr, void(ObjectType::*setSize)(UINT32) = nullptr, UINT64 flags = 0)
		{
			addReflectablePtrArrayField<ObjectType, DataType>(name, uniqueId, 
				std::function<std::shared_ptr<DataType>(ObjectType*, UINT32)>(getter), 
				std::function<UINT32(ObjectType*)>(getSize), 
				std::function<void(ObjectType*, UINT32, std::shared_ptr<DataType>)>(setter), 
				std::function<void(ObjectType*, UINT32)>(setSize), flags);
		}

		template<class ObjectType>
		void addDataBlockField(const String& name, UINT32 uniqueId, ManagedDataBlock (ObjectType::*getter)(), 
			void (ObjectType::*setter)(ManagedDataBlock) = nullptr, UINT64 flags = 0, UINT8* (customAllocator)(ObjectType*, UINT32) = 0)
		{
			addDataBlockField<ObjectType>(name, uniqueId, 
				std::function<ManagedDataBlock(ObjectType*)>(getter),  
				std::function<void(ObjectType*, ManagedDataBlock)>(setter), flags, customAllocator);
		}	

	protected:
		virtual void initSerializableFields() {}

		/************************************************************************/
		/* 		FIELDS OPERATING ON DERIVED SERIALIZATION INTERFACE				*/
		/*		(Needs an extra pointer to the actual object)					*/
		/************************************************************************/
		template<class InterfaceType, class ObjectType, class DataType>
		void addPlainField(const String& name, UINT32 uniqueId, 
			DataType& (InterfaceType::*getter)(ObjectType*), 
			void (InterfaceType::*setter)(ObjectType*, DataType&), UINT64 flags = 0)
		{
			using namespace std::placeholders;

			static_assert((std::is_base_of<CamelotFramework::RTTIType<Type, BaseType, MyRTTIType>, InterfaceType>::value), 
				"Class with the get/set methods must derive from CamelotFramework::RTTIType.");

			static_assert(!(std::is_base_of<CamelotFramework::IReflectable, DataType>::value), 
				"Data type derives from IReflectable but it is being added as a plain field.");

			addPlainField<ObjectType, DataType>(name, uniqueId, 
				std::function<DataType&(ObjectType*)>(std::bind(getter, static_cast<InterfaceType*>(this), _1)), 
				std::function<void(ObjectType*, DataType&)>(std::bind(setter, static_cast<InterfaceType*>(this), _1, _2)), flags);
		}

		template<class InterfaceType, class ObjectType, class DataType>
		void addReflectableField(const String& name, UINT32 uniqueId, 
			DataType& (InterfaceType::*getter)(ObjectType*), 
			void (InterfaceType::*setter)(ObjectType*, DataType&), UINT64 flags = 0)
		{
			using namespace std::placeholders;

			addReflectableField<ObjectType, DataType>(name, uniqueId, 
				std::function<DataType&(ObjectType*)>(std::bind(getter, static_cast<InterfaceType*>(this), _1)), 
				std::function<void(ObjectType*, DataType&)>(std::bind(setter, static_cast<InterfaceType*>(this), _1, _2)), flags);
		}

		template<class InterfaceType, class ObjectType, class DataType>
		void addReflectablePtrField(const String& name, UINT32 uniqueId, 
			std::shared_ptr<DataType> (InterfaceType::*getter)(ObjectType*), 
			void (InterfaceType::*setter)(ObjectType*, std::shared_ptr<DataType>), UINT64 flags = 0)
		{
			using namespace std::placeholders;

			addReflectablePtrField<ObjectType, DataType>(name, uniqueId, 
				std::function<std::shared_ptr<DataType>(ObjectType*)>(std::bind(getter, static_cast<InterfaceType*>(this), _1)), 
				std::function<void(ObjectType*, std::shared_ptr<DataType>)>(std::bind(setter, static_cast<InterfaceType*>(this), _1, _2)), flags);
		}

		template<class InterfaceType, class ObjectType, class DataType>
		void addPlainArrayField(const String& name, UINT32 uniqueId, 
			DataType& (InterfaceType::*getter)(ObjectType*, UINT32), 
			UINT32 (InterfaceType::*getSize)(ObjectType*), 
			void (InterfaceType::*setter)(ObjectType*, UINT32, DataType&), 
			void(InterfaceType::*setSize)(ObjectType*, UINT32), UINT64 flags = 0)
		{
			using namespace std::placeholders;

			static_assert((std::is_base_of<CamelotFramework::RTTIType<Type, BaseType, MyRTTIType>, InterfaceType>::value), 
				"Class with the get/set methods must derive from CamelotFramework::RTTIType.");

			static_assert(!(std::is_base_of<CamelotFramework::IReflectable, DataType>::value), 
				"Data type derives from IReflectable but it is being added as a plain field.");

			addPlainArrayField<ObjectType, DataType>(name, uniqueId, 
				std::function<DataType&(ObjectType*, UINT32)>(std::bind(getter, static_cast<InterfaceType*>(this), _1, _2)), 
				std::function<UINT32(ObjectType*)>(std::bind(getSize, static_cast<InterfaceType*>(this), _1)), 
				std::function<void(ObjectType*, UINT32, DataType&)>(std::bind(setter, static_cast<InterfaceType*>(this), _1, _2, _3)), 
				std::function<void(ObjectType*, UINT32)>(std::bind(setSize, static_cast<InterfaceType*>(this), _1, _2)), flags);
		}	

		template<class InterfaceType, class ObjectType, class DataType>
		void addReflectableArrayField(const String& name, UINT32 uniqueId, 
			DataType& (InterfaceType::*getter)(ObjectType*, UINT32), 
			UINT32 (InterfaceType::*getSize)(ObjectType*), 
			void (InterfaceType::*setter)(ObjectType*, UINT32, DataType&), 
			void(InterfaceType::*setSize)(ObjectType*, UINT32), UINT64 flags = 0)
		{
			using namespace std::placeholders;

			addReflectableArrayField<ObjectType, DataType>(name, uniqueId, 
				std::function<DataType&(ObjectType*, UINT32)>(std::bind(getter, static_cast<InterfaceType*>(this), _1, _2)), 
				std::function<UINT32(ObjectType*)>(std::bind(getSize, static_cast<InterfaceType*>(this), _1)), 
				std::function<void(ObjectType*, UINT32, DataType&)>(std::bind(setter, static_cast<InterfaceType*>(this), _1, _2, _3)), 
				std::function<void(ObjectType*, UINT32)>(std::bind(setSize, static_cast<InterfaceType*>(this), _1, _2)), flags);
		}

		template<class InterfaceType, class ObjectType, class DataType>
		void addReflectablePtrArrayField(const String& name, UINT32 uniqueId, 
			std::shared_ptr<DataType> (InterfaceType::*getter)(ObjectType*, UINT32), 
			UINT32 (InterfaceType::*getSize)(ObjectType*), 
			void (InterfaceType::*setter)(ObjectType*, UINT32, std::shared_ptr<DataType>), 
			void(InterfaceType::*setSize)(ObjectType*, UINT32), UINT64 flags = 0)
		{
			using namespace std::placeholders;

			addReflectablePtrArrayField<ObjectType, DataType>(name, uniqueId, 
				std::function<std::shared_ptr<DataType>(ObjectType*, UINT32)>(std::bind(getter, static_cast<InterfaceType*>(this), _1, _2)), 
				std::function<UINT32(ObjectType*)>(std::bind(getSize, static_cast<InterfaceType*>(this), _1)), 
				std::function<void(ObjectType*, UINT32, std::shared_ptr<DataType>)>(std::bind(setter, static_cast<InterfaceType*>(this), _1, _2, _3)), 
				std::function<void(ObjectType*, UINT32)>(std::bind(setSize, static_cast<InterfaceType*>(this), _1, _2)), flags);
		}

		template<class InterfaceType, class ObjectType>
		void addDataBlockField(const String& name, UINT32 uniqueId, ManagedDataBlock (InterfaceType::*getter)(ObjectType*), 
			void (InterfaceType::*setter)(ObjectType*, ManagedDataBlock), UINT64 flags = 0, 
			UINT8* (customAllocator)(ObjectType*, UINT32) = 0)
		{
			using namespace std::placeholders;

			if(customAllocator != 0)
			{
				std::function<UINT8*(ObjectType*, UINT32)> customAllocFunc = std::bind(customAllocator, _1, _2);

				addDataBlockField<ObjectType>(name, uniqueId, 
					std::function<ManagedDataBlock(ObjectType*)>(std::bind(getter, static_cast<InterfaceType*>(this), _1)),  
					std::function<void(ObjectType*, ManagedDataBlock)>(std::bind(setter, static_cast<InterfaceType*>(this), _1, _2)), flags, 
					customAllocFunc);
			}
			else
			{
				addDataBlockField<ObjectType>(name, uniqueId, 
					std::function<ManagedDataBlock(ObjectType*)>(std::bind(getter, static_cast<InterfaceType*>(this), _1)),  
					std::function<void(ObjectType*, ManagedDataBlock)>(std::bind(setter, static_cast<InterfaceType*>(this), _1, _2)), flags);
			}
		}	

	private:
		template<class ObjectType, class DataType>
		void addPlainField(const String& name, UINT32 uniqueId, boost::any getter, boost::any setter, UINT64 flags)
		{
			RTTIPlainField<DataType, ObjectType>* newField = 
				cm_new<RTTIPlainField<DataType, ObjectType>>();
			newField->initSingle(name, uniqueId, getter, setter, flags);
			addNewField(newField);
		}
		
		template<class ObjectType, class DataType>
		void addReflectableField(const String& name, UINT32 uniqueId, boost::any getter, boost::any setter, UINT64 flags)
		{
			static_assert((std::is_base_of<CamelotFramework::IReflectable, DataType>::value), 
				"Invalid data type for complex field. It needs to derive from CamelotFramework::IReflectable.");

			RTTIReflectableField<DataType, ObjectType>* newField = 
				cm_new<RTTIReflectableField<DataType, ObjectType>>();
			newField->initSingle(name, uniqueId, getter, setter, flags);
			addNewField(newField);
		}

		template<class ObjectType, class DataType>
		void addReflectablePtrField(const String& name, UINT32 uniqueId, boost::any getter, boost::any setter, UINT64 flags)
		{
			static_assert((std::is_base_of<CamelotFramework::IReflectable, DataType>::value), 
				"Invalid data type for complex field. It needs to derive from CamelotFramework::IReflectable.");

			if((flags & RTTI_Flag_WeakRef) == 0)
				checkForCircularReferences<DataType>();

			RTTIReflectablePtrField<DataType, ObjectType>* newField = 
				cm_new<RTTIReflectablePtrField<DataType, ObjectType>>();
			newField->initSingle(name, uniqueId, getter, setter, flags);
			addNewField(newField);
		}

		template<class ObjectType, class DataType>
		void addPlainArrayField(const String& name, UINT32 uniqueId, boost::any getter, boost::any getSize, 
			boost::any setter, boost::any setSize, UINT64 flags)
		{
			RTTIPlainField<DataType, ObjectType>* newField = 
				cm_new<RTTIPlainField<DataType, ObjectType>>();
			newField->initArray(name, uniqueId, getter, getSize, setter, setSize, flags);
			addNewField(newField);
		}	

		template<class ObjectType, class DataType>
		void addReflectableArrayField(const String& name, UINT32 uniqueId, boost::any getter, boost::any getSize, 
			boost::any setter, boost::any setSize, UINT64 flags)
		{
			static_assert((std::is_base_of<CamelotFramework::IReflectable, DataType>::value), 
				"Invalid data type for complex field. It needs to derive from CamelotFramework::IReflectable.");

			RTTIReflectableField<DataType, ObjectType>* newField = 
				cm_new<RTTIReflectableField<DataType, ObjectType>>();
			newField->initArray(name, uniqueId, getter, getSize, setter, setSize, flags);
			addNewField(newField);
		}

		template<class ObjectType, class DataType>
		void addReflectablePtrArrayField(const String& name, UINT32 uniqueId, boost::any getter, boost::any getSize, 
			boost::any setter, boost::any setSize, UINT64 flags)
		{
			static_assert((std::is_base_of<CamelotFramework::IReflectable, DataType>::value), 
				"Invalid data type for complex field. It needs to derive from CamelotFramework::IReflectable.");

			if((flags & RTTI_Flag_WeakRef) == 0)
				checkForCircularReferences<DataType>();

			RTTIReflectablePtrField<DataType, ObjectType>* newField = 
				cm_new<RTTIReflectablePtrField<DataType, ObjectType>>();
			newField->initArray(name, uniqueId, getter, getSize, setter, setSize, flags);
			addNewField(newField);
		}

		template<class ObjectType>
		void addDataBlockField(const String& name, UINT32 uniqueId, boost::any getter, boost::any setter, UINT64 flags, boost::any customAllocator = boost::any())
		{
			RTTIManagedDataBlockField<ManagedDataBlock, ObjectType>* newField = 
				cm_new<RTTIManagedDataBlockField<ManagedDataBlock, ObjectType>>();
			newField->initSingle(name, uniqueId, getter,  setter, flags, customAllocator);
			addNewField(newField);
		}	
	};

	template <typename Type, typename BaseType, typename MyRTTIType>
	InitRTTIOnStart<Type, BaseType> RTTIType<Type, BaseType, MyRTTIType>::initOnStart;

	/**
	 * @brief	Returns true if the provided object can be safely cast into type T.
	 */
	template<class T>
	bool rtti_is_of_type(IReflectable* object)
	{
		static_assert((std::is_base_of<CamelotFramework::IReflectable, T>::value), 
			"Invalid data type for type checking. It needs to derive from CamelotFramework::IReflectable.");

		return object->getTypeId() == T::getRTTIStatic()->getRTTIId();
	}

	/**
	 * @brief	Returns true if the provided object can be safely cast into type T.
	 */
	template<class T>
	bool rtti_is_of_type(std::shared_ptr<IReflectable> object)
	{
		static_assert((std::is_base_of<CamelotFramework::IReflectable, T>::value), 
			"Invalid data type for type checking. It needs to derive from CamelotFramework::IReflectable.");

		return object->getTypeId() == T::getRTTIStatic()->getRTTIId();
	}
}