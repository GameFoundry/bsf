#pragma once

#include "BsPrerequisites.h"
#include "BsSpecificImporter.h"

namespace BansheeEngine
{
	/**
	 * @brief	Imports plain text files (.txt, .xml, .json).
	 */
	class BS_EXPORT PlainTextImporter : public SpecificImporter
	{
	public:
		PlainTextImporter();
		virtual ~PlainTextImporter();

		/** @copydoc SpecificImporter::isExtensionSupported */
		virtual bool isExtensionSupported(const WString& ext) const;

		/** @copydoc SpecificImporter::isMagicNumberSupported */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const;

		/** @copydoc SpecificImporter::import */
		virtual ResourcePtr import(const Path& filePath, ConstImportOptionsPtr importOptions);

		static const WString DEFAULT_EXTENSION;
	};
}