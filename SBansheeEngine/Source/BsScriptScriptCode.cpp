#include "BsScriptScriptCode.h"
#include "BsScriptResourceManager.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsScriptAssemblyManager.h"
#include "BsManagedSerializableObjectInfo.h"
#include <regex>

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptScriptCode::ScriptScriptCode(MonoObject* instance, const HScriptCode& scriptCode)
		:ScriptObject(instance), mScriptCode(scriptCode)
	{
		
	}

	void ScriptScriptCode::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptScriptCode::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_GetText", &ScriptScriptCode::internal_getText);
		metaData.scriptClass->addInternalCall("Internal_SetText", &ScriptScriptCode::internal_setText);
		metaData.scriptClass->addInternalCall("Internal_GetTypes", &ScriptScriptCode::internal_getTypes);
	}

	void ScriptScriptCode::internal_createInstance(MonoObject* instance, MonoString* text)
	{
		WString strText = MonoUtil::monoToWString(text);
		HScriptCode scriptCode = ScriptCode::create(strText);

		ScriptScriptCode* scriptInstance;
		ScriptResourceManager::instance().createScriptResource(instance, scriptCode, &scriptInstance);
	}

	MonoString* ScriptScriptCode::internal_getText(ScriptScriptCode* thisPtr)
	{
		HScriptCode scriptCode = thisPtr->mScriptCode;

		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), scriptCode->getString());
	}

	void ScriptScriptCode::internal_setText(ScriptScriptCode* thisPtr, MonoString* text)
	{
		HScriptCode scriptCode = thisPtr->mScriptCode;

		scriptCode->setString(MonoUtil::monoToWString(text));
	}
	
	MonoArray* ScriptScriptCode::internal_getTypes(ScriptScriptCode* thisPtr)
	{
		Vector<FullTypeName> types;
		if (thisPtr->getScriptCodeHandle().isLoaded())
			types = parseTypes(thisPtr->getScriptCodeHandle()->getString());

		Vector<MonoReflectionType*> validTypes;
		for (auto& type : types)
		{
			ManagedSerializableObjectInfoPtr objInfo;
			if (ScriptAssemblyManager::instance().getSerializableObjectInfo(toString(type.first), toString(type.second), objInfo))
			{
				MonoType* monoType = mono_class_get_type(objInfo->mTypeInfo->getMonoClass());
				validTypes.push_back(mono_type_get_object(MonoManager::instance().getDomain(), monoType));
			}
		}

		UINT32 numValidTypes = (UINT32)validTypes.size();

		MonoClass* typeClass = ScriptAssemblyManager::instance().getSystemTypeClass();
		MonoArray* output = mono_array_new(MonoManager::instance().getDomain(), typeClass->_getInternalClass(), numValidTypes);

		for (UINT32 i = 0; i < numValidTypes; i++)
			mono_array_set(output, MonoReflectionType*, i, validTypes[i]);

		return output;
	}

	void ScriptScriptCode::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;

		if (!mRefreshInProgress)
			ScriptResourceManager::instance().destroyScriptResource(this);
	}

	void ScriptScriptCode::setNativeHandle(const HResource& resource)
	{
		mScriptCode = static_resource_cast<ScriptCode>(resource);
	}

	Vector<ScriptScriptCode::FullTypeName> ScriptScriptCode::parseTypes(const WString& code)
	{
		struct NamespaceData
		{
			WString ns;
			INT32 bracketIdx;
		};

		Vector<FullTypeName> output;
		Stack<NamespaceData> namespaces;

		// Note: Won't match unicode escape sequences
		WString identifierPattern = LR"([_@a-zA-Z][\p{Lu}\p{Ll}\p{Lt}\p{Lm}\p{Lo}\p{Nl}\p{Mn}\p{Mc}\p{Nd}\p{Pc}\p{Cf}]*)";
		std::wregex identifierRegex(identifierPattern, std::regex_constants::ECMAScript | std::regex_constants::icase);

		WString nsToken = L"namespace";
		WString classToken = L"class";

		UINT32 idx = 0;
		INT32 bracketIdx = 0;
		for (auto iter = code.begin(); iter != code.end(); ++iter)
		{
			wchar_t ch = *iter;
			
			if (code.compare(idx, classToken.size(), classToken))
			{
				std::wsmatch results;
				if (std::regex_match(iter + classToken.size(), code.end(), results, identifierRegex))
				{
					WString ns = L"";
					if (!namespaces.empty())
						ns = namespaces.top().ns;

					std::wstring tempStr = results[0];
					WString typeName = tempStr.c_str();

					output.push_back(FullTypeName());
					FullTypeName& nsTypePair = output.back();
					nsTypePair.first = ns;
					nsTypePair.second = typeName;
				}
			}
			else if (code.compare(idx, nsToken.size(), nsToken))
			{
				std::wsmatch results;
				if (std::regex_match(iter + nsToken.size(), code.end(), results, identifierRegex))
				{
					std::wstring tempStr = results[0];
					WString ns = tempStr.c_str();

					namespaces.push({ ns, bracketIdx + 1 });
				}
			}
			else if (ch == '{')
			{
				bracketIdx++;
			}
			else if (ch == '}')
			{
				bracketIdx--;
			}

			idx++;
		}

		return output;
	}
}