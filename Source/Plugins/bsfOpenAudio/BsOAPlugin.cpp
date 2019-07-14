//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsOAPrerequisites.h"
#include "Audio/BsAudioManager.h"
#include "BsOAAudio.h"
#include "BsOAImporter.h"
#include "Importer/BsImporter.h"

namespace bs
{
	class OAFactory : public AudioFactory
	{
	public:
		void startUp() override
		{
			Audio::startUp<OAAudio>();
		}

		void shutDown() override
		{
			Audio::shutDown();
		}
	};

	/**	Returns a name of the plugin. */
	extern "C" BS_PLUGIN_EXPORT const char* getPluginName()
	{
		static const char* pluginName = "OpenAudio";
		return pluginName;
	}

	/**	Entry point to the plugin. Called by the engine when the plugin is loaded. */
	extern "C" BS_PLUGIN_EXPORT void* loadPlugin()
	{
		OAImporter* importer = bs_new<OAImporter>();
		Importer::instance()._registerAssetImporter(importer);

		return bs_new<OAFactory>();
	}

	/**	Exit point of the plugin. Called by the engine before the plugin is unloaded. */
	extern "C" BS_PLUGIN_EXPORT void unloadPlugin(OAFactory* instance)
	{
		bs_delete(instance);
	}
}
