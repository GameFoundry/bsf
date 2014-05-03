#pragma once

#include "CmPrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Abstract class that is to be specialized in converting
	 * 			a certain asset type into an engine usable resource. 
	 * 			(e.g. a .png file into an engine usable texture).
	 * 			
	 *			On initialization this class must register itself with the Importer module,
	 *			which delegates asset import calls to a specific importer.
	 */
	class CM_EXPORT SpecificImporter
	{
	public:
		SpecificImporter() {}
		virtual ~SpecificImporter() {}

		/**
		 * @brief	Check is the provided extension supported by this importer.
		 *
		 * @note	Provided extension should be without the leading dot.
		 */
		virtual bool isExtensionSupported(const WString& ext) const = 0;

		/**
		 * @brief	Check if the provided magic number is supported by this importer.
		 */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const = 0; 

		/**
		 * @brief	Imports the given file.
		 *
		 * @param	filePath	Pathname of the file, with file extension.
		 *
		 * @return	null if it fails, otherwise the loaded object.
		 */
		virtual ResourcePtr import(const Path& filePath, ConstImportOptionsPtr importOptions) = 0;

		/**
		 * @brief	Creates import options specific for this importer. Import
		 * 			options are provided when calling import() in order to customize the
		 * 			import, and provide additional information.
		 */
		virtual ImportOptionsPtr createImportOptions() const;

		/**
		 * @brief	Gets the default import options.
		 *
		 * @return	The default import options.
		 */
		ConstImportOptionsPtr getDefaultImportOptions() const;

	private:
		mutable ConstImportOptionsPtr mDefaultImportOptions;
	};
}