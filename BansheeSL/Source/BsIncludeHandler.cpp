#include "BsIncludeHandler.h"
#include "BsSLPrerequisites.h"
#include "BsShaderManager.h"
#include "BsShaderInclude.h"
#include "BsMMAlloc.h"

using namespace BansheeEngine;

char* includePush(ParseState* state, const char* filename, int line, int column, int* size)
{
	HShaderInclude include = ShaderManager::instance().findInclude(filename);

	if (include != nullptr)
		include.blockUntilLoaded();

	int filenameLen = (int)strlen(filename);
	if (include.isLoaded())
	{
		String includeSource = include->getString();

		*size = (int)includeSource.size() + 2;
		int totalSize = *size + sizeof(IncludeLink) + sizeof(IncludeData) + filenameLen + 1;
		char* output = (char*)mmalloc(state->memContext, totalSize);
		char* ptr = output;

		memcpy(ptr, includeSource.data(), *size - 2);
		ptr[*size - 2] = 0;
		ptr[*size - 1] = 0;

		ptr += *size;
		IncludeLink* next = state->includeStack;

		IncludeLink* newLink = (IncludeLink*)ptr;
		ptr += sizeof(IncludeLink);

		IncludeData* includeData = (IncludeData*)ptr;
		ptr += sizeof(IncludeData);

		memcpy(ptr, filename, filenameLen);
		ptr[filenameLen] = '\0';

		includeData->filename = ptr; 
		includeData->buffer = output;

		newLink->data = includeData;
		newLink->next = next;

		state->includeStack = newLink;
		
		return output;
	}

	const char* errorLabel = "Error opening include file :";
	int labelLen = (int)strlen(errorLabel);

	int messageLen = filenameLen + labelLen + 1;
	char* message = (char*)mmalloc(state->memContext, messageLen);

	memcpy(message, filename, filenameLen);
	memcpy(message + filenameLen, errorLabel, labelLen);
	message[messageLen - 1] = '\0';

	state->hasError = 1;
	state->errorLine = line;
	state->errorColumn = column;
	state->errorMessage = message;

	return nullptr;
}

void includePop(ParseState* state)
{
	IncludeLink* current = state->includeStack;

	if (!current)
		return;

	state->includeStack = current->next;
	mmfree(current->data->buffer);
}