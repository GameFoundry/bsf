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
		ScriptTexture2D* createScriptTexture2D(const HTexture& resourceHandle);

		/**
		* @note Throws an exception if resource for the handle already exists.
		* 		 Creates a new managed instance of the object.
		*/
		ScriptTexture3D* createScriptTexture3D(const HTexture& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Creates a new managed instance of the object.
		 */
		ScriptTextureCube* createScriptTextureCube(const HTexture& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Initializes the ScriptResource with an existing managed instance.
		 */
		ScriptTexture2D* createScriptTexture2D(MonoObject* existingInstance, const HTexture& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Initializes the ScriptResource with an existing managed instance.
		 */
		ScriptTexture3D* createScriptTexture3D(MonoObject* existingInstance, const HTexture& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Initializes the ScriptResource with an existing managed instance.
		 */
		ScriptTextureCube* createScriptTextureCube(MonoObject* existingInstance, const HTexture& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Initializes the ScriptResource with an existing managed instance.
		 */
		ScriptShader* createScriptShader(const HShader& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Initializes the ScriptResource with an existing managed instance.
		 */
		ScriptShader* createScriptShader(MonoObject* existingInstance, const HShader& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Initializes the ScriptResource with an existing managed instance.
		 */
		ScriptMaterial* createScriptMaterial(const HMaterial& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Initializes the ScriptResource with an existing managed instance.
		 */
		ScriptMaterial* createScriptMaterial(MonoObject* existingInstance, const HMaterial& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Initializes the ScriptResource with an existing managed instance.
		 */
		ScriptMesh* createScriptMesh(const HMesh& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Initializes the ScriptResource with an existing managed instance.
		 */
		ScriptMesh* createScriptMesh(MonoObject* existingInstance, const HMesh& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Initializes the ScriptResource with an existing managed instance.
		 */
		ScriptPlainText* createScriptPlainText(const HPlainText& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Initializes the ScriptResource with an existing managed instance.
		 */
		ScriptPlainText* createScriptPlainText(MonoObject* existingInstance, const HPlainText& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Initializes the ScriptResource with an existing managed instance.
		 */
		ScriptScriptCode* createScriptScriptCode(const HScriptCode& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Initializes the ScriptResource with an existing managed instance.
		 */
		ScriptScriptCode* createScriptScriptCode(MonoObject* existingInstance, const HScriptCode& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Creates a new managed instance of the object.
		 */
		ScriptSpriteTexture* createScriptSpriteTexture(const HSpriteTexture& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Initializes the ScriptResource with an existing managed instance.
		 */
		ScriptSpriteTexture* createScriptSpriteTexture(MonoObject* existingInstance, const HSpriteTexture& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Creates a new managed instance of the object.
		 */
		ScriptFont* createScriptFont(const HFont& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Initializes the ScriptResource with an existing managed instance.
		 */
		ScriptFont* createScriptFont(MonoObject* existingInstance, const HFont& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Creates a new managed instance of the object.
		 */
		ScriptPrefab* createScriptPrefab(const HPrefab& resourceHandle);

		/**
		 * @note Throws an exception if resource for the handle already exists.
		 * 		 Initializes the ScriptResource with an existing managed instance.
		 */
		ScriptPrefab* createScriptPrefab(MonoObject* existingInstance, const HPrefab& resourceHandle);

		/**
		* @note  Throws an exception if resource for the handle already exists.
		* 		 Initializes the ScriptResource with an existing managed instance.
		*/
		ScriptManagedResource* createManagedResource(MonoObject* existingInstance, const HManagedResource& resourceHandle);

		/**
		 * @note Returns nullptr if script resource doesn't exist.
		 */
		ScriptTexture2D* getScriptTexture2D(const HTexture& resourceHandle);

		/**
		 * @note Returns nullptr if script resource doesn't exist.
		 */
		ScriptTexture3D* getScriptTexture3D(const HTexture& resourceHandle);

		/**
		 * @note Returns nullptr if script resource doesn't exist.
		 */
		ScriptTextureCube* getScriptTextureCube(const HTexture& resourceHandle);

		/**
		 * @note Returns nullptr if script resource doesn't exist.
		 */
		ScriptShader* getScriptShader(const HShader& resourceHandle);

		/**
		 * @note Returns nullptr if script resource doesn't exist.
		 */
		ScriptMaterial* getScriptMaterial(const HMaterial& resourceHandle);

		/**
		 * @note Returns nullptr if script resource doesn't exist.
		 */
		ScriptMesh* getScriptMesh(const HMesh& resourceHandle);

		/**
		 * @note Returns nullptr if script resource doesn't exist.
		 */
		ScriptSpriteTexture* getScriptSpriteTexture(const HSpriteTexture& resourceHandle);

		/**
		 * @note Returns nullptr if script resource doesn't exist.
		 */
		ScriptPlainText* getScriptPlainText(const HPlainText& resourceHandle);

		/**
		 * @note Returns nullptr if script resource doesn't exist.
		 */
		ScriptScriptCode* getScriptScriptCode(const HScriptCode& resourceHandle);

		/**
		 * @note Returns nullptr if script resource doesn't exist.
		 */
		ScriptManagedResource* getScriptManagedResource(const HManagedResource& resourceHandle);

		/**
		 * @note Returns nullptr if script resource doesn't exist.
		 */
		ScriptPrefab* getScriptPrefab(const HPrefab& resourceHandle);

		/**
		 * @note Returns nullptr if script resource doesn't exist.
		 */
		ScriptResourceBase* getScriptResource(const String& UUID);

		/**
		 * @note Returns nullptr if script resource doesn't exist.
		 */
		ScriptResourceBase* createScriptResource(const HResource& resource);

		void destroyScriptResource(ScriptResourceBase* resource);

	private:
		UnorderedMap<String, ScriptResourceBase*> mScriptResources;

		void throwExceptionIfInvalidOrDuplicate(const String& uuid) const;
	};
}