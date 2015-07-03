#include "BsScriptEditorBuiltin.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsBuiltinEditorResources.h"
#include "BsScriptSpriteTexture.h"
#include "BsScriptResourceManager.h"

namespace BansheeEngine
{
	ScriptEditorBuiltin::ScriptEditorBuiltin(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptEditorBuiltin::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetFolderIcon", &ScriptEditorBuiltin::internal_getFolderIcon);
		metaData.scriptClass->addInternalCall("Internal_GetMeshIcon", &ScriptEditorBuiltin::internal_getMeshIcon);
		metaData.scriptClass->addInternalCall("Internal_GetFontIcon", &ScriptEditorBuiltin::internal_getFontIcon);
		metaData.scriptClass->addInternalCall("Internal_GetTextureIcon", &ScriptEditorBuiltin::internal_getTextureIcon);
		metaData.scriptClass->addInternalCall("Internal_GetPlainTextIcon", &ScriptEditorBuiltin::internal_getPlainTextIcon);
		metaData.scriptClass->addInternalCall("Internal_GetScriptCodeIcon", &ScriptEditorBuiltin::internal_getScriptCodeIcon);
		metaData.scriptClass->addInternalCall("Internal_GetShaderIcon", &ScriptEditorBuiltin::internal_getShaderIcon);
		metaData.scriptClass->addInternalCall("Internal_GetShaderIncludeIcon", &ScriptEditorBuiltin::internal_getShaderIncludeIcon);
		metaData.scriptClass->addInternalCall("Internal_GetMaterialIcon", &ScriptEditorBuiltin::internal_getMaterialIcon);
		metaData.scriptClass->addInternalCall("Internal_GetSpriteTextureIcon", &ScriptEditorBuiltin::internal_getSpriteTextureIcon);
		metaData.scriptClass->addInternalCall("Internal_GetPrefabIcon", &ScriptEditorBuiltin::internal_getPrefabIcon);
	}

	MonoObject* ScriptEditorBuiltin::internal_getFolderIcon()
	{
		HSpriteTexture tex = BuiltinEditorResources::instance().getLibraryIcon(ProjectIcon::Folder);

		return ScriptSpriteTexture::toManaged(tex);
	}

	MonoObject* ScriptEditorBuiltin::internal_getMeshIcon()
	{
		HSpriteTexture tex = BuiltinEditorResources::instance().getLibraryIcon(ProjectIcon::Mesh);

		return ScriptSpriteTexture::toManaged(tex);
	}

	MonoObject* ScriptEditorBuiltin::internal_getFontIcon()
	{
		HSpriteTexture tex = BuiltinEditorResources::instance().getLibraryIcon(ProjectIcon::Font);

		return ScriptSpriteTexture::toManaged(tex);
	}

	MonoObject* ScriptEditorBuiltin::internal_getTextureIcon()
	{
		HSpriteTexture tex = BuiltinEditorResources::instance().getLibraryIcon(ProjectIcon::Texture);

		return ScriptSpriteTexture::toManaged(tex);
	}

	MonoObject* ScriptEditorBuiltin::internal_getPlainTextIcon()
	{
		HSpriteTexture tex = BuiltinEditorResources::instance().getLibraryIcon(ProjectIcon::PlainText);

		return ScriptSpriteTexture::toManaged(tex);
	}

	MonoObject* ScriptEditorBuiltin::internal_getScriptCodeIcon()
	{
		HSpriteTexture tex = BuiltinEditorResources::instance().getLibraryIcon(ProjectIcon::ScriptCode);

		return ScriptSpriteTexture::toManaged(tex);
	}

	MonoObject* ScriptEditorBuiltin::internal_getShaderIcon()
	{
		HSpriteTexture tex = BuiltinEditorResources::instance().getLibraryIcon(ProjectIcon::Shader);

		return ScriptSpriteTexture::toManaged(tex);
	}

	MonoObject* ScriptEditorBuiltin::internal_getShaderIncludeIcon()
	{
		HSpriteTexture tex = BuiltinEditorResources::instance().getLibraryIcon(ProjectIcon::ShaderInclude);

		return ScriptSpriteTexture::toManaged(tex);
	}

	MonoObject* ScriptEditorBuiltin::internal_getMaterialIcon()
	{
		HSpriteTexture tex = BuiltinEditorResources::instance().getLibraryIcon(ProjectIcon::Material);

		return ScriptSpriteTexture::toManaged(tex);
	}

	MonoObject* ScriptEditorBuiltin::internal_getSpriteTextureIcon()
	{
		HSpriteTexture tex = BuiltinEditorResources::instance().getLibraryIcon(ProjectIcon::ScriptCode);

		return ScriptSpriteTexture::toManaged(tex);
	}

	MonoObject* ScriptEditorBuiltin::internal_getPrefabIcon()
	{
		HSpriteTexture tex = BuiltinEditorResources::instance().getLibraryIcon(ProjectIcon::Prefab);

		return ScriptSpriteTexture::toManaged(tex);
	}
}