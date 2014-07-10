//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Module responsible for importing various asset types and converting
	 * 			them to types usable by the engine.
	 */
	class BS_CORE_EXPORT Importer : public Module<Importer>
	{
	public:
		Importer(); 
		~Importer(); 

		/**
		 * @brief	Imports a resource at the specified location, and returns the loaded data.
		 *
		 * @param	inputFilePath	Pathname of the input file.
		 * @param	importOptions	(optional) Options for controlling the import. Caller must
		 *							ensure import options actually match the type of the importer used
		 *							for the file type.
		 *
		 * @see		createImportOptions
		 */
		HResource import(const Path& inputFilePath, ConstImportOptionsPtr importOptions = nullptr);

		/**
		 * @copydoc import
		 */
		template <class T>
		ResourceHandle<T> import(const Path& inputFilePath, ConstImportOptionsPtr importOptions = nullptr)
		{
			return static_resource_cast<T>(import(inputFilePath, importOptions));
		}

		/**
		 * @brief	Imports a resource and replaces the contents of the provided existing resource with new imported data.
		 *
		 * @param	inputFilePath	Pathname of the input file.
		 * @param	importOptions	(optional) Options for controlling the import. Caller must
		 *							ensure import options actually match the type of the importer used
		 *							for the file type. 
		 *
		 * @see		createImportOptions
		 */
		void reimport(HResource& existingResource, const Path& inputFilePath, ConstImportOptionsPtr importOptions = nullptr);

		/**
		 * @brief	Automatically detects the importer needed for the provided file and returns valid type of
		 * 			import options for that importer.
		 *
		 * @param	inputFilePath	Pathname of the input file.
		 *
		 * @return	The new import options.
		 * 			
		 * @note	You will need to type cast the importer options to a valid type,
		 * 			taking into consideration exact importer you expect to be used for this file type.
		 * 			If you don't use a proper import options type, an exception will be thrown during import.
		 * 			
		 *			nullptr is returned if the file path is not valid, or if a valid importer cannot be found for
		 *			the specified file.
		 */
		ImportOptionsPtr createImportOptions(const Path& inputFilePath);

		/**
		 * @brief	Checks if we can import a file with the specified extension.
		 *
		 * @param	extension	The extension without the leading dot.
		 */
		bool supportsFileType(const WString& extension) const;

		/**
		 * @brief	Checks if we can import a file with the specified magic number.
		 *
		 * @param	magicNumber 	The buffer containing the magic number.
		 * @param	magicNumSize	Size of the magic number buffer.
		 */
		bool supportsFileType(const UINT8* magicNumber, UINT32 magicNumSize) const;

		/**
		 * @brief	Adds a new asset importer for the specified file extension. If an asset importer for that extension
		 * 			already exists, it is removed and replaced with the current one.
		 * 			
		 * @note	Internal method. This method should only be called by asset importers themselves on startup.
		 *
		 * @param [in]	importer	The importer that is able to handle files with the specified extension. nullptr if you
		 * 							want to remove an asset importer for the extension.
		 */
		void _registerAssetImporter(SpecificImporter* importer);
	private:
		Vector<SpecificImporter*> mAssetImporters;

		SpecificImporter* getImporterForFile(const Path& inputFilePath) const;
	};
}