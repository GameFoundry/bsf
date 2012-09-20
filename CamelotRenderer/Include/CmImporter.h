#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace CamelotEngine
{
	/**
	 * @brief	Module responsible for importing various asset types and converting
	 * 			them to types usable by the engine.
	 */
	class Importer : public Module<Importer>
	{
	public:
		/**
		 * @brief	Constructor.
		 *
		 * @param	assetDatabasePath	Pathname of the asset database file. Path should not include file extension.
		 * 								If the database file doesn't exist it will be created in that location.
		 * 								Meta data for imported resources will be stored in the asset database.
		 */
		Importer(const String& assetDatabasePath); 
		~Importer(); 

		/**
		 * @brief	Imports a resource at the specified location, and saves the imported data into the
		 * 			output location. This data can then be loaded with Resources.load. Existing files
		 * 			will be replaced.
		 *
		 * @param	inputFilePath 	Pathname of the input file.
		 * @param	outputFilePath	Pathname of the output file, without extension.
		 * @param	keepReferences	(optional) If file is being replaced we can keep all references to it and
		 * 							just make sure they point to the new file data (default), or we can invalidate all
		 * 							references and treat it as a completely new file. 
		 * 							
		 *							If you're just updating a file keep this set to true, and if you're
		 * 							replacing a file, set it to false. 
		 * 							In order for the references to be kept asset types must match.
		 */
		void import(const String& inputFilePath, const String& outputFilePath, bool keepReferences = true);


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