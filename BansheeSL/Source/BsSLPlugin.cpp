#include "BsSLPrerequisites.h"
#include "BsPath.h"
#include "BsFileSystem.h"
#include "BsDataStream.h"

extern "C" {
#include "BsASTFX.h"
#include "BsParserFX.h"
#include "BsLexerFX.h"
}

namespace BansheeEngine
{
	ASTFXNode* parseFX(const char* source)
	{
		yyscan_t scanner;
		YY_BUFFER_STATE state;

		if (yylex_init(&scanner)) {
			// couldn't initialize
			return nullptr;
		}

		state = yy_scan_string(source, scanner);

		ASTFXNode* output = nodeCreate(NT_Shader);
		if (yyparse(output, scanner)) {
			// error parsing
			return nullptr;
		}

		yy_delete_buffer(state, scanner);

		yylex_destroy(scanner);

		return output;
	}

	/**
	 * @brief	Returns a name of the plugin.
	 */
	extern "C" BS_SL_EXPORT const String& getPluginName()
	{
		return SystemName;
	}

	/**
	 * @brief	Entry point to the plugin. Called by the engine when the plugin is loaded.
	 */
	extern "C" BS_SL_EXPORT void* loadPlugin()
	{
		// TODO - Register importer

		// DEBUG
		yydebug = 1;
		Path exampleFX = "D:\\testFX.txt";
		DataStreamPtr file = FileSystem::openFile(exampleFX);

		String contents = file->getAsString();
		ASTFXNode* node = parseFX(contents.c_str());

		int bp = 0;

		nodeDelete(node);

		return nullptr;
	}
}