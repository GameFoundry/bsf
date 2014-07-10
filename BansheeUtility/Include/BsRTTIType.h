//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include <string>
#include <algorithm>
#include <unordered_map>

#include "BsPrerequisitesUtil.h"
#include "BsManagedDataBlock.h"
#include "BsRTTIField.h"
#include "BsRTTIPlainField.h"
#include "BsRTTIReflectableField.h"
#include "BsRTTIReflectablePtrField.h"
#include "BsRTTIManagedDataBlockField.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
#define BS_SETGET_MEMBER(name, type, parentType)								\
	type##& get##name(parentType##* obj) { return obj->##name; }				\
	void Set##name(parentType##* obj, type##& val) { obj->##name = val; } 

#define BS_ADD_PLAINFIELD(name, id, parentType) \
	addPlainField(#name, id##, &##parentType##::get##name, &##parentType##::Set##name);

	/**
	 * @brief	Provides an interface for accessing fields of a certain class.
	 * 			Data can be easily accessed by getter and setter methods.
	 * 			
	 *			Supported data types:
	 *			 - Plain types - All types defined in BsRTTIField.h, mostly native types and POD (plain old data) structs. Data is parsed byte by byte.  
	 *			                 No pointers to plain types are supported. Data is passed around by value.
	 *			 - Reflectable types - Any class deriving from IReflectable. Data is parsed based on fields in its RTTI class. Can be pointer or value type.
	 *			 - Arrays of both plain and reflectable types are supported
	 *			 - Data blocks - A managed or unmanaged block of data. See BsManagedDataBlock.h
	 */
	class BS_UTILITY_EXPORT RTTITypeBase
	{
	public:
		RTTITypeBase();
		virtual ~RTTITypeBase();

		/**
		 * @brief	Returns RTTI type information for all classes that derive from the class
		 *			that owns this RTTI type.
		 */
		virtual Vector<RTTITypeBase*>& getDerivedClasses() = 0;

		/**
		 * @brief	Returns RTTI type information for the class that owns this RTTI type. 
		 *			If the class has not base type, null is returned instead.
		 */
		virtual RTTITypeBase* getBaseClass() = 0;

		/**
		 * @brief	Internal method. Called by the RTTI system when a class is first found in
		 *			order to form child/parent class hierarchy.
		 */
		virtual void _registerDerivedClass(RTTITypeBase* derivedClass) = 0;

		/**
		 * @brief	Creates a new instance of the class owning this RTTI type.
		 */
		virtual std::shared_ptr<IReflectable> newRTTIObject() = 0;

		/**
		 * @brief	Returns the name of the class owning this RTTI type.
		 */
		virtual const String& getRTTIName() = 0;

		/**
		 * @brief	Returns an RTTI id that uniquely represents each class in the RTTI
		 *			system.
		 */
		virtual UINT32 getRTTIId() = 0;

		/**
		 * @brief	Called by the serializers when serialization for this object has started.
		 *			Use this to do any preprocessing on data you might need during serialization itself.
		 */
		virtual void onSerializationStarted(IReflectable* obj) {}

		/**
		 * @brief	Called by the serializers when serialization for this object has ended.
		 *			After serialization has ended you can be sure that the type has been fully serialized,
		 *			and you may clean up any temporary data.
		 */
		virtual void onSerializationEnded(IReflectable* obj) {}

		/**
		 * @brief	Called by the serializers when deserialization for this object has started.
		 *			Use this to do any preprocessing on data you might need during deserialization itself.
		 */
		virtual void onDeserializationStarted(IReflectable* obj) {}

		/**
		 * @brief	Called by the serializers when deserialization for this object has ended.
		 *			At this point you can be sure the instance has been fully deserialized and you
		 *			may safely use it.
		 *
		 *			One exception being are fields you marked with "WeakRef" flag, as they might be resolved
		 *			only after deserialization has fully completed for all objects.
		 */
		virtual void onDeserializationEnded(IReflectable* obj) {}

		/**
		 * @brief	Allows you to assign a value to a plain field with the specified name on 
		 *			the provided instance.
		 *
		 * @note	Caller must ensure instance and value types are valid for this field.
		 */
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

		/**
		 * @brief	Allows you to assign a value to a plain field array element with the 
		 *			specified name and index on the provided instance.
		 *
		 * @note	Caller must ensure instance and value types are valid for this field.
		 */
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

		/**
		 * @brief	Allows you to assign a value to a reflectable field with the specified name on 
		 *			the provided instance.
		 *
		 * @note	Caller must ensure instance and value types are valid for this field.
		 */
		template <class ObjectType, class DataType>
		void setReflectableValue(ObjectType* object, const String& name, DataType& value)
		{
			static_assert((std::is_base_of<BansheeEngine::IReflectable, DataType>::value), 
				"Invalid data type for complex field. It needs to derive from BansheeEngine::IReflectable.");

			RTTIField* genericField = findField(name);
			genericField->checkIsComplex(false);

			RTTIReflectableFieldBase* field = static_cast<RTTIReflectableFieldBase*>(genericField);
			field->setValue(object, value);
		}

		/**
		 * @brief	Allows you to assign a value to a reflectable field array element with the 
		 *			specified name and index on the provided instance.
		 *
		 * @note	Caller must ensure instance and value types are valid for this field.
		 */
		template <class ObjectType, class DataType>
		void setReflectableArrayValue(ObjectType* object, const String& name, UINT32 index, DataType& value)
		{
			static_assert((std::is_base_of<BansheeEngine::IReflectable, DataType>::value), 
				"Invalid data type for complex field. It needs to derive from BansheeEngine::IReflectable.");

			RTTIField* genericField = findField(name);
			genericField->checkIsComplex(true);

			RTTIReflectableFieldBase* field = static_cast<RTTIReflectableFieldBase*>(genericField);
			field->setArrayValue(object, index, value);
		}

		/**
		 * @brief	Allows you to assign a value to a managed data block field with the specified name on 
		 *			the provided instance.
		 *
		 * @note	Caller must ensure instance type is valid for this field.
		 */
		template <class ObjectType>
		void setDataBlockValue(ObjectType* object, const String& name, ManagedDataBlock value)
		{
			RTTIField* genericField = findField(name);
			genericField->checkIsDataBlock();

			RTTIManagedDataBlockFieldBase* field = static_cast<RTTIManagedDataBlockFieldBase*>(genericField);
			field->setValue(object, value);
		}

		/**
		 * @brief	Allows you to assign a value to a reflectable pointer field with the specified name on 
		 *			the provided instance.
		 *
		 * @note	Caller must ensure instance and value types are valid for this field.
		 */
		template <class ObjectType, class DataType>
		void setReflectablePtrValue(ObjectType* object, const String& name, std::shared_ptr<DataType> value)
		{
			static_assert((std::is_base_of<BansheeEngine::IReflectable, DataType>::value), 
				"Invalid data type for complex field. It needs to derive from BansheeEngine::IReflectable.");

			RTTIField* genericField = findField(name);
			genericField->checkIsComplexPtr(false);

			RTTIReflectablePtrFieldBase* field = static_cast<RTTIReflectablePtrFieldBase*>(genericField);
			field->setValue(object, value);
		}

		/**
		 * @brief	Allows you to assign a value to a reflectable pointer field array element with the 
		 *			specified name and index on the provided instance.
		 *
		 * @note	Caller must ensure instance and value types are valid for this field.
		 */
		template <class ObjectType, class DataType>
		void setReflectablePtrArrayValue(ObjectType* object, const String& name, UINT32 index, std::shared_ptr<DataType> value)
		{
			static_assert((std::is_base_of<BansheeEngine::IReflectable, DataType>::value), 
				"Invalid data type for complex field. It needs to derive from BansheeEngine::IReflectable.");

			RTTIField* genericField = findField(name);
			genericField->checkIsComplexPtr(true);

			RTTIReflectablePtrFieldBase* field = static_cast<RTTIReflectablePtrFieldBase*>(genericField);
			field->setArrayValue(object, index, value);
		}

		/**
		 * @brief	Reads a value from a plain field with the specified name from the provided instance.
		 *
		 * @note	Caller must ensure instance and value types are valid for this field.
		 */
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

		/**
		 * @brief	Reads a value from a plain array field with the specified name and index from the provided instance.
		 *
		 * @note	Caller must ensure instance and value types are valid for this field.
		 */
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

		/**
		 * @brief	Reads a value from a reflectable object field with the specified name from the provided instance.
		 *
		 * @note	Caller must ensure instance and value types are valid for this field.
		 */
		template <class ObjectType>
		IReflectable& getReflectableValue(ObjectType* object, const String& name)
		{
			RTTIField* genericField = findField(name);
			genericField->checkIsComplex(false);

			RTTIReflectableFieldBase* field = static_cast<RTTIReflectableFieldBase*>(genericField);
			return field->getValue(object);
		}

		/**
		 * @brief	Reads a value from a reflectable object array field with the specified name and index from the provided instance.
		 *
		 * @note	Caller must ensure instance and value types are valid for this field.
		 */
		template <class ObjectType>
		IReflectable& getReflectableArrayValue(ObjectType* object, const String& name, UINT32 index)
		{
			RTTIField* genericField = findField(name);
			genericField->checkIsComplex(true);

			RTTIReflectableFieldBase* field = static_cast<RTTIReflectableFieldBase*>(genericField);
			return field->getArrayValue(object, index);
		}

		/**
		 * @brief	Reads a managed data block field with the specified name from the provided instance.
		 *
		 * @note	Caller must ensure instance type is valid for this field.
		 */
		template <class ObjectType>
		ManagedDataBlock getDataBlockValue(ObjectType* object, const String& name)
		{
			RTTIField* genericField = findField(name);
			genericField->checkIsDataBlock();

			RTTIManagedDataBlockFieldBase* field = static_cast<RTTIManagedDataBlockFieldBase*>(genericField);
			return field->getValue(object);
		}

		/**
		 * @brief	Reads a value from a reflectable object pointer field with the specified name from the provided instance.
		 *
		 * @note	Caller must ensure instance and value types are valid for this field.
		 */
		template <class ObjectType>
		std::shared_ptr<IReflectable> getReflectablePtrValue(ObjectType* object, const String& name)
		{
			RTTIField* genericField = findField(name);
			genericField->checkIsComplexPtr(false);

			RTTIReflectablePtrFieldBase* field = static_cast<RTTIReflectablePtrFieldBase*>(genericField);
			return field->getValue(object);
		}

		/**
		 * @brief	Reads a value from a reflectable pointer array field with the specified name and index from the provided instance.
		 *
		 * @note	Caller must ensure instance and value types are valid for this field.
		 */
		template <class ObjectType>
		std::shared_ptr<IReflectable> getReflectablePtrArrayValue(ObjectType* object, const String& name, UINT32 index)
		{
			RTTIField* genericField = findField(name);
			genericField->checkIsComplexPtr(true);

			RTTIReflectablePtrFieldBase* field = static_cast<RTTIReflectablePtrFieldBase*>(genericField);
			return field->getArrayValue(object, index);
		}

		/**
		 * @brief	Returns the size of the array of the field with the specified name on the provided instance.
		 *
		 * @note	Caller must ensure instance type is valid and that the field as an array.
		 */
		template <class ObjectType>
		UINT32 getArraySize(ObjectType* object, const String& name)
		{
			RTTIField* field = findField(name);
			return field->getArraySize(object);
		}

		/**
		 * @brief	Sets the size of the array of the field with the specified name on the provided instance.
		 *
		 * @note	Caller must ensure instance type is valid and that the field as an array. 
		 *			This might clear any existing data from the array.
		 */
		template <class ObjectType>
		void setArraySize(ObjectType* object, const String& name, UINT32 size)
		{
			RTTIField* field = findField(name);
			field->setArraySize(object, size);
		}	

		/**
		 * @brief	Returns the total number of fields in this RTTI type.
		 */
		UINT32 getNumFields() { return (UINT32)mFields.size(); }

		/**
		 * @brief	Returns a field based on the field index. Use "getNumFields" to
		 *			get total number of fields available.
		 */
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
		 * @note Only a separate function so I don't need to include BsException header.
		 */
		void throwCircularRefException(const String& myType, const String& otherType) const;

	private:
		Vector<RTTIField*> mFields;
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
			BaseType::getRTTIStatic()->_registerDerivedClass(Type::getRTTIStatic());
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
			IReflectable::_registerDerivedClass(Type::getRTTIStatic());
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

	/**
	 * @brief	Specialization for root class of RTTI hierarchy - IReflectable
	 */
	template<>
	struct GetRTTIType<IReflectable>
	{
		RTTITypeBase* operator()() { return nullptr; }
	};

	/**
	 * @brief	Allows you to provide a run-time type information for a specific class, along with 
	 * 			support for serialization/deserialization.
	 * 			
	 * 			Derive from this class and return the that class from IReflectable::getRTTI. 
	 * 			This way you can separate serialization logic from the actual class you're serializing.
	 *
	 *			This class will provide a way to register individual fields in the class, together with ways to
	 *			read and write them, as well a providing information about class hierarchy, and run-time type checking.
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

		/**
		 * @brief	Returns a singleton of this RTTI type.
		 */
		static MyRTTIType* instance()
		{
			static MyRTTIType inst;
			return &inst;
		}

		/**
		 * @copydoc	RTTITypeBase::getDerivedClasses
		 */
		virtual Vector<RTTITypeBase*>& getDerivedClasses()
		{
			static Vector<RTTITypeBase*> mRTTIDerivedClasses;
			return mRTTIDerivedClasses;
		}

		/**
		 * @copydoc	RTTITypeBase::getBaseClass
		 */
		virtual RTTITypeBase* getBaseClass()
		{
			return GetRTTIType<BaseType>()();
		}

		/**
		 * @copydoc	RTTITypeBase::_registerDerivedClass
		 */
		virtual void _registerDerivedClass(RTTITypeBase* derivedClass)
		{
			if(IReflectable::_isTypeIdDuplicate(derivedClass->getRTTIId()))
			{
				BS_EXCEPT(InternalErrorException, "RTTI type \"" + derivedClass->getRTTIName() + 
					"\" has a duplicate ID: " + toString(derivedClass->getRTTIId()));
			}

			getDerivedClasses().push_back(derivedClass);
		}

		/************************************************************************/
		/* 			FIELDS OPERATING DIRECTLY ON SERIALIZABLE OBJECT            */
		/************************************************************************/

		/**
		 * @brief	Registers a new plain field. This field can then be accessed dynamically from the RTTI system and
		 *			used for automatic serialization. See RTTIField for more information about field types.
		 *
		 * @param	name		Name of the field.
		 * @param	uniqueId	Unique identifier for this field. Although name is also a unique
		 * 						identifier we want a small data type that can be used for efficiently
		 * 						serializing data to disk and similar. It is primarily used for compatibility
		 * 						between different versions of serialized data.
		 * @param	getter  	Method used for retrieving the value of this field.
		 * @param	setter  	Method used for setting the value of this field.
		 * @param	flags		Various flags you can use to specialize how systems handle this field. See RTTIFieldFlag.
		 */
		template<class ObjectType, class DataType>
		void addPlainField(const String& name, UINT32 uniqueId, DataType& (ObjectType::*getter)(), 
			void (ObjectType::*setter)(DataType&) = nullptr, UINT64 flags = 0)
		{
			addPlainField<ObjectType, DataType>(name, uniqueId, 
				std::function<DataType&(ObjectType*)>(getter), 
				std::function<void(ObjectType*, DataType&)>(setter), flags);
		}

		/**
		 * @brief	Registers a new reflectable object field. This field can then be accessed dynamically from the RTTI system and
		 *			used for automatic serialization. See RTTIField for more information about field types.
		 *
		 * @param	name		Name of the field.
		 * @param	uniqueId	Unique identifier for this field. Although name is also a unique
		 * 						identifier we want a small data type that can be used for efficiently
		 * 						serializing data to disk and similar. It is primarily used for compatibility
		 * 						between different versions of serialized data.
		 * @param	getter  	Method used for retrieving the value of this field.
		 * @param	setter  	Method used for setting the value of this field.
		 * @param	flags		Various flags you can use to specialize how systems handle this field. See RTTIFieldFlag.
		 */
		template<class ObjectType, class DataType>
		void addReflectableField(const String& name, UINT32 uniqueId, DataType& (ObjectType::*getter)(), 
			void (ObjectType::*setter)(DataType&) = nullptr, UINT64 flags = 0)
		{
			addReflectableField<ObjectType, DataType>(name, uniqueId, 
				std::function<DataType&(ObjectType*)>(getter), 
				std::function<void(ObjectType*, DataType&)>(setter), flags);
		}

		/**
		 * @brief	Registers a new reflectable object pointer field. This field can then be accessed dynamically from the RTTI system and
		 *			used for automatic serialization. See RTTIField for more information about field types.
		 *
		 * @param	name		Name of the field.
		 * @param	uniqueId	Unique identifier for this field. Although name is also a unique
		 * 						identifier we want a small data type that can be used for efficiently
		 * 						serializing data to disk and similar. It is primarily used for compatibility
		 * 						between different versions of serialized data.
		 * @param	getter  	Method used for retrieving the value of this field.
		 * @param	setter  	Method used for setting the value of this field.
		 * @param	flags		Various flags you can use to specialize how systems handle this field. See RTTIFieldFlag.
		 */
		template<class ObjectType, class DataType>
		void addReflectablePtrField(const String& name, UINT32 uniqueId, std::shared_ptr<DataType> (ObjectType::*getter)(), 
			void (ObjectType::*setter)(std::shared_ptr<DataType>) = nullptr, UINT64 flags = 0)
		{
			addReflectablePtrField<ObjectType, DataType>(name, uniqueId, 
				std::function<std::shared_ptr<DataType>(ObjectType*)>(getter), 
				std::function<void(ObjectType*, std::shared_ptr<DataType>)>(setter), flags);
		}

		/**
		 * @brief	Registers a new field containg an array of plain values. This field can then be accessed dynamically from the RTTI system and
		 *			used for automatic serialization. See RTTIField for more information about field types.
		 *
		 * @param	name		Name of the field.
		 * @param	uniqueId	Unique identifier for this field. Although name is also a unique
		 * 						identifier we want a small data type that can be used for efficiently
		 * 						serializing data to disk and similar. It is primarily used for compatibility
		 * 						between different versions of serialized data.
		 * @param	getter  	Method used for retrieving a single element of the array.
		 * @param	getSize 	Getter method that returns the size of the array.
		 * @param	setter  	Method used for setting the a single element of the field.
		 * @param	setSize 	Setter method that allows you to resize the array. 
		 * @param	flags		Various flags you can use to specialize how systems handle this field. See RTTIFieldFlag.
		 */
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

		/**
		 * @brief	Registers a new field containg an array of reflectable object values. This field can then be accessed 
		 *			dynamically from the RTTI system and used for automatic serialization. See RTTIField for more information 
		 *			about field types.
		 *
		 * @param	name		Name of the field.
		 * @param	uniqueId	Unique identifier for this field. Although name is also a unique
		 * 						identifier we want a small data type that can be used for efficiently
		 * 						serializing data to disk and similar. It is primarily used for compatibility
		 * 						between different versions of serialized data.
		 * @param	getter  	Method used for retrieving a single element of the array.
		 * @param	getSize 	Getter method that returns the size of the array.
		 * @param	setter  	Method used for setting the a single element of the field.
		 * @param	setSize 	Setter method that allows you to resize the array. 
		 * @param	flags		Various flags you can use to specialize how systems handle this field. See RTTIFieldFlag.
		 */
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

		/**
		 * @brief	Registers a new field containg an array of reflectable obejct pointers. This field can then be accessed 
		 *			dynamically from the RTTI system and used for automatic serialization. See RTTIField for more information 
		 *			about field types.
		 *
		 * @param	name		Name of the field.
		 * @param	uniqueId	Unique identifier for this field. Although name is also a unique
		 * 						identifier we want a small data type that can be used for efficiently
		 * 						serializing data to disk and similar. It is primarily used for compatibility
		 * 						between different versions of serialized data.
		 * @param	getter  	Method used for retrieving a single element of the array.
		 * @param	getSize 	Getter method that returns the size of the array.
		 * @param	setter  	Method used for setting the a single element of the field.
		 * @param	setSize 	Setter method that allows you to resize the array. 
		 * @param	flags		Various flags you can use to specialize how systems handle this field. See RTTIFieldFlag.
		 */
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

		/**
		 * @brief	Registers a new managed data block field. This field can then be accessed dynamically from the RTTI system and
		 *			used for automatic serialization. See RTTIField for more information about field types.
		 *
		 * @param	name		Name of the field.
		 * @param	uniqueId	Unique identifier for this field. Although name is also a unique
		 * 						identifier we want a small data type that can be used for efficiently
		 * 						serializing data to disk and similar. It is primarily used for compatibility
		 * 						between different versions of serialized data.
		 * @param	getter  	Method used for retrieving the value of this field.
		 * @param	setter  	Method used for setting the value of this field.
		 * @param	flags		Various flags you can use to specialize how systems handle this field. See RTTIFieldFlag.
		 */
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

			static_assert((std::is_base_of<BansheeEngine::RTTIType<Type, BaseType, MyRTTIType>, InterfaceType>::value), 
				"Class with the get/set methods must derive from BansheeEngine::RTTIType.");

			static_assert(!(std::is_base_of<BansheeEngine::IReflectable, DataType>::value), 
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

			static_assert((std::is_base_of<BansheeEngine::RTTIType<Type, BaseType, MyRTTIType>, InterfaceType>::value), 
				"Class with the get/set methods must derive from BansheeEngine::RTTIType.");

			static_assert(!(std::is_base_of<BansheeEngine::IReflectable, DataType>::value), 
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
		void addPlainField(const String& name, UINT32 uniqueId, Any getter, Any setter, UINT64 flags)
		{
			RTTIPlainField<DataType, ObjectType>* newField = 
				bs_new<RTTIPlainField<DataType, ObjectType>>();
			newField->initSingle(name, uniqueId, getter, setter, flags);
			addNewField(newField);
		}
		
		template<class ObjectType, class DataType>
		void addReflectableField(const String& name, UINT32 uniqueId, Any getter, Any setter, UINT64 flags)
		{
			static_assert((std::is_base_of<BansheeEngine::IReflectable, DataType>::value), 
				"Invalid data type for complex field. It needs to derive from BansheeEngine::IReflectable.");

			RTTIReflectableField<DataType, ObjectType>* newField = 
				bs_new<RTTIReflectableField<DataType, ObjectType>>();
			newField->initSingle(name, uniqueId, getter, setter, flags);
			addNewField(newField);
		}

		template<class ObjectType, class DataType>
		void addReflectablePtrField(const String& name, UINT32 uniqueId, Any getter, Any setter, UINT64 flags)
		{
			static_assert((std::is_base_of<BansheeEngine::IReflectable, DataType>::value), 
				"Invalid data type for complex field. It needs to derive from BansheeEngine::IReflectable.");

			if((flags & RTTI_Flag_WeakRef) == 0)
				checkForCircularReferences<DataType>();

			RTTIReflectablePtrField<DataType, ObjectType>* newField = 
				bs_new<RTTIReflectablePtrField<DataType, ObjectType>>();
			newField->initSingle(name, uniqueId, getter, setter, flags);
			addNewField(newField);
		}

		template<class ObjectType, class DataType>
		void addPlainArrayField(const String& name, UINT32 uniqueId, Any getter, Any getSize,
			Any setter, Any setSize, UINT64 flags)
		{
			RTTIPlainField<DataType, ObjectType>* newField = 
				bs_new<RTTIPlainField<DataType, ObjectType>>();
			newField->initArray(name, uniqueId, getter, getSize, setter, setSize, flags);
			addNewField(newField);
		}	

		template<class ObjectType, class DataType>
		void addReflectableArrayField(const String& name, UINT32 uniqueId, Any getter, Any getSize,
			Any setter, Any setSize, UINT64 flags)
		{
			static_assert((std::is_base_of<BansheeEngine::IReflectable, DataType>::value), 
				"Invalid data type for complex field. It needs to derive from BansheeEngine::IReflectable.");

			RTTIReflectableField<DataType, ObjectType>* newField = 
				bs_new<RTTIReflectableField<DataType, ObjectType>>();
			newField->initArray(name, uniqueId, getter, getSize, setter, setSize, flags);
			addNewField(newField);
		}

		template<class ObjectType, class DataType>
		void addReflectablePtrArrayField(const String& name, UINT32 uniqueId, Any getter, Any getSize,
			Any setter, Any setSize, UINT64 flags)
		{
			static_assert((std::is_base_of<BansheeEngine::IReflectable, DataType>::value), 
				"Invalid data type for complex field. It needs to derive from BansheeEngine::IReflectable.");

			if((flags & RTTI_Flag_WeakRef) == 0)
				checkForCircularReferences<DataType>();

			RTTIReflectablePtrField<DataType, ObjectType>* newField = 
				bs_new<RTTIReflectablePtrField<DataType, ObjectType>>();
			newField->initArray(name, uniqueId, getter, getSize, setter, setSize, flags);
			addNewField(newField);
		}

		template<class ObjectType>
		void addDataBlockField(const String& name, UINT32 uniqueId, Any getter, Any setter, UINT64 flags, 
			Any customAllocator = Any())
		{
			RTTIManagedDataBlockField<ManagedDataBlock, ObjectType>* newField = 
				bs_new<RTTIManagedDataBlockField<ManagedDataBlock, ObjectType>>();
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
		static_assert((std::is_base_of<BansheeEngine::IReflectable, T>::value), 
			"Invalid data type for type checking. It needs to derive from BansheeEngine::IReflectable.");

		return object->getTypeId() == T::getRTTIStatic()->getRTTIId();
	}

	/**
	 * @brief	Returns true if the provided object can be safely cast into type T.
	 */
	template<class T>
	bool rtti_is_of_type(std::shared_ptr<IReflectable> object)
	{
		static_assert((std::is_base_of<BansheeEngine::IReflectable, T>::value), 
			"Invalid data type for type checking. It needs to derive from BansheeEngine::IReflectable.");

		return object->getTypeId() == T::getRTTIStatic()->getRTTIId();
	}
}