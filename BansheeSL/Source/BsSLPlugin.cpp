#include "BsSLPrerequisites.h"
#include "BsPath.h"
#include "BsFileSystem.h"
#include "BsDataStream.h"

extern "C" {
#include "BsMMAlloc.h"
#include "BsASTFX.h"
#include "BsParserFX.h"
#include "BsLexerFX.h"
}

namespace BansheeEngine
{
	void parseFX(ParseState* parseState, const char* source)
	{
		yyscan_t scanner;
		YY_BUFFER_STATE state;

		if (yylex_init_extra(parseState, &scanner)) {
			// couldn't initialize
			return;
		}

		state = yy_scan_string(source, scanner);

		if (yyparse(parseState, scanner)) {
			// error parsing
			return;
		}

		yy_delete_buffer(state, scanner);

		yylex_destroy(scanner);
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

		ParseState* parseState = parseStateCreate();
		parseFX(parseState, contents.c_str());

		int bp = 0;

		parseStateDelete(parseState);

		return nullptr;
	}
}