#pragma once

#include "BsEditorPrerequisites.h"
#include "BsSpecificImporter.h"

namespace BansheeEngine
{
	/**
	 * @brief	Imports built-in Banshee resources (marked with .asset extension). 
	 *			Essentially just does a pass-through as source asset to import and 
	 *			resulting resource are one and the same.
	 *
	 * @note	This is useful in the project library where we sometimes want to save processed
	 *			resources in the library.
	 */
	class BS_ED_EXPORT ResourceImporter : public SpecificImporter
	{
	public:
		ResourceImporter();
		virtual ~ResourceImporter();

		/** @copydoc SpecificImporter::isExtensionSupported */
		virtual bool isExtensionSupported(const WString& ext) const override;

		/** @copydoc SpecificImporter::isMagicNumberSupported */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const override;

		/** @copydoc SpecificImporter::import */
		virtual ResourcePtr import(const Path& filePath, ConstImportOptionsPtr importOptions) override;

		static const WString DEFAULT_EXTENSION;
	};
}