#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsModule.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptResourceManager : public Module<ScriptResourceManager>
	{
	public:
		ScriptResourceManager();

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Creates a new managed instance of the object.
		 */
		template<class RetType, class InType>
		void createScriptResource(const ResourceHandle<InType>& resourceHandle, RetType** out);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Initializes the ScriptResource with an existing managed instance.
		 */
		template<class RetType, class InType>
		void createScriptResource(MonoObject* existingInstance, const ResourceHandle<InType>& resourceHandle, RetType** out);

		/**
		 * @note If @p create is true, creates a new script resource if one doesn't exist, 
		 *		 otherwise returns nullptr if script resource doesn't exist.
		 */
		template<class RetType, class InType>
		void getScriptResource(const ResourceHandle<InType>& resourceHandle, RetType** out, bool create = false);

		template<>
		void createScriptResource(const ResourceHandle<StringTable>& resourceHandle, ScriptStringTable** out);

		template<>
		void createScriptResource(const HResource& resourceHandle, ScriptResourceBase** out);

		/**
		 * @note Returns nullptr if script resource doesn't exist.
		 */
		ScriptResourceBase* getScriptResource(const String& UUID);

		void destroyScriptResource(ScriptResourceBase* resource);

	private:
		UnorderedMap<String, ScriptResourceBase*> mScriptResources;

		void throwExceptionIfInvalidOrDuplicate(const String& uuid) const;
	};
}