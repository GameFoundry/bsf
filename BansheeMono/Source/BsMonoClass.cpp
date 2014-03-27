#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoField.h"
#include "BsMonoProperty.h"
#include "BsMonoAssembly.h"
#include "BsMonoManager.h"
#include "CmUtil.h"
#include "CmException.h"
#include <mono/metadata/debug-helpers.h>

using namespace CamelotFramework;

namespace BansheeEngine
{
	inline size_t MonoClass::MethodId::Hash::operator()(const MonoClass::MethodId& v) const
	{
		size_t seed = 0;
		hash_combine(seed, v.name);
		hash_combine(seed, v.numParams);
		return seed;
	}

	inline bool MonoClass::MethodId::Equals::operator()(const MonoClass::MethodId &a, const MonoClass::MethodId &b) const
	{
		return a.name == b.name && a.numParams == b.numParams;
	}

	MonoClass::MethodId::MethodId(const String& name, UINT32 numParams)
		:name(name), numParams(numParams)
	{
		
	}

	MonoClass::MonoClass(const String& ns, const String& type, ::MonoClass* monoClass, const MonoAssembly* parentAssembly)
		:mNamespace(ns), mTypeName(type), mClass(monoClass), mParentAssembly(parentAssembly), mHasCachedFields(false)
	{
		mFullName = ns + "." + type;
	}

	MonoClass::~MonoClass()
	{
		for(auto& mapEntry : mMethods)
		{
			cm_delete(mapEntry.second);
		}

		mMethods.clear();

		for(auto& mapEntry : mFields)
		{
			cm_delete(mapEntry.second);
		}

		mFields.clear();

		for(auto& mapEntry : mProperties)
		{
			cm_delete(mapEntry.second);
		}

		mProperties.clear();
	}

	MonoMethod& MonoClass::getMethod(const String& name, UINT32 numParams)
	{
		MethodId mehodId(name, numParams);
		auto iterFind = mMethods.find(mehodId);
		if(iterFind != mMethods.end())
			return *iterFind->second;

		::MonoMethod* method = mono_class_get_method_from_name(mClass, name.c_str(), (int)numParams);
		if(method == nullptr)
		{
			String fullMethodName = mFullName + "::" + name;
			CM_EXCEPT(InvalidParametersException, "Cannot get Mono method: " + fullMethodName);
		}

		MonoMethod* newMethod = new (cm_alloc<MonoMethod>()) MonoMethod(method);
		mMethods[mehodId] = newMethod;

		return *newMethod;
	}

	MonoMethod* MonoClass::getMethodExact(const CM::String& name, const CM::String& signature)
	{
		MethodId mehodId(name + "(" + signature + ")", 0);
		auto iterFind = mMethods.find(mehodId);
		if(iterFind != mMethods.end())
			return iterFind->second;

		::MonoMethod* method;
		void* iter = nullptr;

		const char* rawName = name.c_str();
		const char* rawSig = signature.c_str();
		while ((method = mono_class_get_methods(mClass, &iter)))
		{
			if (strcmp(rawName, mono_method_get_name(method)) == 0)
			{
				const char* curSig = mono_signature_get_desc(mono_method_signature(method), false);
				if(strcmp(rawSig, curSig) == 0)
				{
					MonoMethod* newMethod = new (cm_alloc<MonoMethod>()) MonoMethod(method);
					mMethods[mehodId] = newMethod;

					return newMethod;
				}
			}
		}

		return nullptr;
	}

	bool MonoClass::hasField(const String& name) const
	{
		MonoClassField* field = mono_class_get_field_from_name(mClass, name.c_str());

		return field != nullptr;
	}

	bool MonoClass::isSubClassOf(const BS::MonoClass* monoClass) const
	{
		if(monoClass == nullptr)
			return false;

		return mono_class_is_subclass_of(mClass, monoClass->mClass, true) != 0;
	}

	MonoField* MonoClass::getField(const String& name) const
	{
		auto iterFind = mFields.find(name);
		if(iterFind != mFields.end())
			return iterFind->second;

		MonoClassField* field = mono_class_get_field_from_name(mClass, name.c_str());
		if(field == nullptr)
			return nullptr;

		MonoField* newField = new (cm_alloc<MonoField>()) MonoField(field);
		mFields[name] = newField;

		return newField;
	}

	MonoProperty& MonoClass::getProperty(const String& name)
	{
		auto iterFind = mProperties.find(name);
		if(iterFind != mProperties.end())
			return *iterFind->second;

		::MonoProperty* property = mono_class_get_property_from_name(mClass, name.c_str());
		if(property == nullptr)
		{
			String fullPropertyName = mFullName + "." + name;
			CM_EXCEPT(InvalidParametersException, "Cannot get Mono property: " + fullPropertyName);
		}

		MonoProperty* newProperty = new (cm_alloc<MonoProperty>()) MonoProperty(property);
		mProperties[name] = newProperty;

		return *newProperty;
	}

	const CM::Vector<MonoField*>::type MonoClass::getAllFields() const
	{
		if(mHasCachedFields)
			return mCachedFieldList;

		mCachedFieldList.clear();

		void* iter = nullptr;
		MonoClassField* curClassField = mono_class_get_fields(mClass, &iter);
		while(curClassField != nullptr)
		{
			const char* fieldName = mono_field_get_name(curClassField);
			MonoField* curField = getField(fieldName);

			mCachedFieldList.push_back(curField);

			curClassField = mono_class_get_fields(mClass, &iter);
		}

		mHasCachedFields = true;
		return mCachedFieldList;
	}

	MonoObject* MonoClass::invokeMethod(const String& name, MonoObject* instance, void** params, UINT32 numParams)
	{
		return getMethod(name, numParams).invoke(instance, params);
	}

	void MonoClass::addInternalCall(const String& name, const void* method)
	{
		String fullMethodName = mFullName + "::" + name;
		mono_add_internal_call(fullMethodName.c_str(), method);
	}

	MonoObject* MonoClass::createInstance(bool construct) const
	{
		MonoObject* obj = mono_object_new(MonoManager::instance().getDomain(), mClass);

		if(construct)
			mono_runtime_object_init(obj);

		return obj;
	}

	MonoObject* MonoClass::createInstance(void** params, UINT32 numParams)
	{
		MonoObject* obj = mono_object_new(MonoManager::instance().getDomain(), mClass);
		getMethod(".ctor", numParams).invoke(obj, params);

		return obj;
	}

	MonoObject* MonoClass::createInstance(const CM::String& ctorSignature, void** params)
	{
		MonoObject* obj = mono_object_new(MonoManager::instance().getDomain(), mClass);
		getMethodExact(".ctor", ctorSignature)->invoke(obj, params);

		return obj;
	}

	bool MonoClass::hasAttribute(MonoClass* monoClass) const
	{
		// TODO - Consider caching custom attributes or just initializing them all at load

		MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_class(mClass);
		if(attrInfo == nullptr)
			return false;

		bool hasAttr = mono_custom_attrs_has_attr(attrInfo, monoClass->_getInternalClass()) != 0;

		mono_custom_attrs_free(attrInfo);

		return hasAttr;
	}

	MonoObject* MonoClass::getAttribute(MonoClass* monoClass) const
	{
		// TODO - Consider caching custom attributes or just initializing them all at load

		MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_class(mClass);
		if(attrInfo == nullptr)
			return nullptr;

		MonoObject* foundAttr = mono_custom_attrs_get_attr(attrInfo, monoClass->_getInternalClass());

		mono_custom_attrs_free(attrInfo);

		return foundAttr;
	}

	MonoClass* MonoClass::getBaseClass() const
	{
		::MonoClass* monoBase = mono_class_get_parent(mClass);
		if(monoBase == nullptr)
			return nullptr;

		String ns = mono_class_get_namespace(monoBase);
		String typeName = mono_class_get_name(monoBase);

		return MonoManager::instance().findClass(ns, typeName);
	}

	bool MonoClass::isInstanceOfType(MonoObject* object) const
	{
		if(object == nullptr)
			return false;

		::MonoClass* monoClass = mono_object_get_class(object);
		return mono_class_is_subclass_of(monoClass, mClass, false) != 0;
	}
}