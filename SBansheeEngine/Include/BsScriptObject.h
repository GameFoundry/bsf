#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "CmException.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	template <class Type>
	struct InitScriptObjectOnStart
	{
	public:
		InitScriptObjectOnStart()
		{
			Type::initMetaData();
		}

		void makeSureIAmInstantiated() { }
	};

	/**
	 * @brief	 Base class for objects that can be extended using Mono scripting
	 */
	template <class Type>
	class ScriptObject
	{
	public:
		ScriptObject()
			:mManagedInstance(nullptr)
		{	
			// Compiler will only generate code for stuff that is directly used, including static data members,
			// so we fool it here like we're using the class directly. Otherwise compiler won't generate the code for the member
			// and our type won't get initialized on start (Actual behavior is a bit more random)
			initOnStart.makeSureIAmInstantiated();
		}

		virtual ~ScriptObject() 
		{
			if(mManagedInstance != nullptr)
				CM_EXCEPT(InvalidStateException, "Script object is being destroyed without its instance previously being released.");
		}

		MonoObject* getManagedInstance() const { return mManagedInstance; }
		virtual void* getNativeRaw() const { return nullptr; }

		static Type* toNative(MonoObject* managedInstance)
		{
			Type* nativeInstance = nullptr;
			metaData.thisPtrField->getValue(managedInstance, &nativeInstance);

			return nativeInstance;
		}

		static const ScriptMeta* getMetaData() { return &metaData; }

	protected:
		static ScriptMeta metaData;

		MonoObject* mManagedInstance;

		void createInstance(MonoObject* instance)
		{
			if(mManagedInstance != nullptr)
				CM_EXCEPT(InvalidStateException, "Trying to instantiate an already instantiated script object.");

			mManagedInstance = instance;
		}

		template <class Type2>
		static void throwIfInstancesDontMatch(ScriptObject<Type2>* lhs, void* rhs)
		{
#if CM_DEBUG_MODE
			if((lhs == nullptr && rhs != nullptr) || (rhs == nullptr && lhs != nullptr) || lhs->getNativeRaw() != rhs)
			{
				CM_EXCEPT(InvalidStateException, "Native and script instance do not match. This usually happens when you modify a native object " \
					" that is also being referenced from script code. You should only modify such objects directly from script code.");
			}
#endif
		}

	private:
		static InitScriptObjectOnStart<Type> initOnStart;
	};

	template <typename Type>
	InitScriptObjectOnStart<Type> ScriptObject<Type>::initOnStart;

	template <typename Type>
	ScriptMeta ScriptObject<Type>::metaData;
}