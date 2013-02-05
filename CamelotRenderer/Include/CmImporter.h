#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace CamelotEngine
{
	/**
	 * @brief	Module responsible for importing various asset types and converting
	 * 			them to types usable by the engine.
	 */
	class CM_EXPORT Importer : public Module<Importer>
	{
	public:
		/**
		 * @brief	Constructor.
		 */
		Importer(); 
		~Importer(); 

		/**
		 * @brief	Imports a resource at the specified location, and returns the loaded data.
		 *
		 * @param	inputFilePath 	Pathname of the input file.
		 */
		BaseResourceHandle import(const String& inputFilePath);

		/**
		 * @brief	Checks if we can import a file with the specified extension.
		 *
		 * @param	extension	The extension without leading dot.
		 */
		bool supportsFileType(const std::string& extension) const;

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
		 * @note	This method should only be called by asset importers themselves on startup.
		 *
		 * @param [in]	importer	The importer that is able to handle files with the specified extension. nullptr if you
		 * 							want to remove an asset importer for the extension.
		 */
		void registerAssetImporter(SpecificImporter* importer);
	private:
		vector<SpecificImporter*>::type mAssetImporters;
	};
}