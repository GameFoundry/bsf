#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsGameResourceManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles loading of game resources when the editor is running.
	 */
	class BS_SCR_BED_EXPORT EditorResourceLoader : public IGameResourceLoader
	{
	public:
		/**
		 * @copydoc	IGameResourceLoader::load
		 */
		HResource load(const Path& path, bool keepLoaded) const override;
	};
}