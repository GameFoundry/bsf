#pragma once

#include "BsPrerequisites.h"
#include "BsScriptMeta.h"
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
			:mInstanceCreated(false)
		{	
			// Compiler will only generate code for stuff that is directly used, including static data members,
			// so we fool it here like we're using the class directly. Otherwise compiler won't generate the code for the member
			// and our type won't get initialized on start (Actual behavior is a bit more random)
			initOnStart.makeSureIAmInstantiated();
		}

		virtual ~ScriptObject() 
		{
			if(mInstanceCreated)
				CM_EXCEPT(InvalidStateException, "Script object is being destroyed without its instance previously being released.");
		}

	protected:
		static ScriptMeta metaData;

		bool mInstanceCreated;

		void createInstance()
		{
			if(mInstanceCreated)
				CM_EXCEPT(InvalidStateException, "Trying to instantiate an already instantiated script object.");

			mInstanceCreated = true;
		}

		void destroyInstance()
		{
			if(!mInstanceCreated)
				return;

			mInstanceCreated = false;
		}

	private:
		static InitScriptObjectOnStart<Type> initOnStart;
	};

	template <typename Type>
	InitScriptObjectOnStart<Type> ScriptObject<Type>::initOnStart;

	template <typename Type>
	ScriptMeta ScriptObject<Type>::metaData;
}