//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/** @addtogroup Importer
	 *  @{
	 */

	/** Module responsible for importing various asset types and converting them to types usable by the engine. */
	class BS_CORE_EXPORT Importer : public Module<Importer>
	{
	public:
		Importer(); 
		~Importer(); 

		/**
		 * Imports a resource at the specified location, and returns the loaded data.
		 *
		 * @param[in]	inputFilePath	Pathname of the input file.
		 * @param[in]	importOptions	(optional) Options for controlling the import. Caller must ensure import options 
		 *								actually match the type of the importer used for the file type.
		 *
		 * @see		createImportOptions
		 */
		HResource import(const Path& inputFilePath, ConstImportOptionsPtr importOptions = nullptr);

		/** @copydoc import */
		template <class T>
		ResourceHandle<T> import(const Path& inputFilePath, ConstImportOptionsPtr importOptions = nullptr)
		{
			return static_resource_cast<T>(import(inputFilePath, importOptions));
		}

		/**
		 * Imports a resource and replaces the contents of the provided existing resource with new imported data.
		 *
		 * @param[in]	inputFilePath	Pathname of the input file.
		 * @param[in]	importOptions	(optional) Options for controlling the import. Caller must ensure import options 
		 *								actually match the type of the importer used for the file type. 
		 *
		 * @see		createImportOptions
		 */
		void reimport(HResource& existingResource, const Path& inputFilePath, ConstImportOptionsPtr importOptions = nullptr);

		/**
		 * Automatically detects the importer needed for the provided file and returns valid type of import options for 
		 * that importer.
		 *
		 * @param[in]	inputFilePath	Pathname of the input file.
		 *
		 * @return						The new import options. Null is returned if the file path is not valid, or if a 
		 *								valid importer cannot be found for the specified file.
		 * 			
		 * @note	
		 * You will need to type cast the importer options to a valid type, taking into consideration exact importer you 
		 * expect to be used for this file type. If you don't use a proper import options type, an exception will be thrown 
		 * during import.
		 */
		ImportOptionsPtr createImportOptions(const Path& inputFilePath);

		/** @copydoc createImportOptions */
		template<class T>
		SPtr<T> createImportOptions(const Path& inputFilePath)
		{
			return std::static_pointer_cast<T>(createImportOptions(inputFilePath));
		}

		/**
		 * Checks if we can import a file with the specified extension.
		 *
		 * @param[in]	extension	The extension without the leading dot.
		 */
		bool supportsFileType(const WString& extension) const;

		/**
		 * Checks if we can import a file with the specified magic number.
		 *
		 * @param[in]	magicNumber 	The buffer containing the magic number.
		 * @param[in]	magicNumSize	Size of the magic number buffer.
		 */
		bool supportsFileType(const UINT8* magicNumber, UINT32 magicNumSize) const;

		/**
		 * Adds a new asset importer for the specified file extension. If an asset importer for that extension already 
		 * exists, it is removed and replaced with the current one.
		 *
		 *
		 * @param [in]	importer	The importer that is able to handle files with the specified extension. nullptr if you
		 * 							want to remove an asset importer for the extension.
		 *
		 * @note	Internal method. 
		 * @note	This method should only be called by asset importers themselves on startup. Importer takes ownership
		 *			of the provided pointer and will release it. Assumes it is allocated using the general allocator.
		 */
		void _registerAssetImporter(SpecificImporter* importer);
	private:
		Vector<SpecificImporter*> mAssetImporters;

		SpecificImporter* getImporterForFile(const Path& inputFilePath) const;
	};

	/** Provides easier access to Importer. */
	BS_CORE_EXPORT Importer& gImporter();

	/** @} */
}