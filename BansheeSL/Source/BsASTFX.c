#include "BsASTFX.h"
#include "BsMMAlloc.h"

OptionInfo OPTION_LOOKUP[] =
{
	{ OT_Node, ODT_Complex },
	{ OT_Separable, ODT_Bool },
	{ OT_Priority, ODT_Int },
	{ OT_Queue, ODT_Int },
};

NodeOptions* nodeOptionsCreate(void* context)
{
	static const int BUFFER_SIZE = 5;

	NodeOptions* options = (NodeOptions*)mmalloc(context, sizeof(NodeOptions));
	options->count = 0;
	options->bufferSize = BUFFER_SIZE;

	options->entries = (NodeOption*)mmalloc(context, sizeof(NodeOption) * options->bufferSize);
	memset(options->entries, 0, sizeof(NodeOption) * options->bufferSize);

	return options;
}

void nodeOptionDelete(NodeOption* option)
{
	if (OPTION_LOOKUP[(int)option->type].dataType == ODT_Complex)
	{
		mmfree(option->value.complexValue); // TODO - Maybe this has more complex delete logic? (It probably will have)
		option->value.complexValue = 0;
	}
	else if (OPTION_LOOKUP[(int)option->type].dataType == ODT_String)
	{
		mmfree((void*)option->value.strValue);
		option->value.strValue = 0;
	}
}

void nodeOptionsDelete(NodeOptions* options)
{
	int i = 0;

	for (i = 0; i < options->count; i++)
		nodeOptionDelete(&options->entries[i]);

	mmfree(options->entries);
	mmfree(options);
}

void nodeOptionsResize(void* context, NodeOptions* options, int size)
{
	NodeOption* originalEntries = options->entries;
	int originalSize = options->bufferSize;
	int originalCount = options->count;
	int i = 0;
	int elementsToCopy = originalSize;
	int sizeToCopy = 0;

	options->bufferSize = size;
	if (options->count > options->bufferSize)
		options->count = options->bufferSize;

	if (elementsToCopy > size)
		elementsToCopy = size;

	sizeToCopy = elementsToCopy * sizeof(NodeOption);

	options->entries = (NodeOption*)mmalloc(context, sizeof(NodeOption) * options->bufferSize);

	memcpy(options->entries, originalEntries, sizeToCopy);
	memset(options->entries + elementsToCopy, 0, sizeof(NodeOption) * options->bufferSize - sizeToCopy);

	for (i = 0; i < originalCount; i++)
		nodeOptionDelete(&originalEntries[i]);

	mmfree(originalEntries);
}

void nodeOptionsGrowIfNeeded(void* context, NodeOptions* options)
{
	static const int BUFFER_GROW = 10;

	if (options->count == options->bufferSize)
		nodeOptionsResize(context, options, options->bufferSize + BUFFER_GROW);
}

void nodeOptionsAdd(void* context, NodeOptions* options, NodeOption* option)
{
	nodeOptionsGrowIfNeeded(context, options);

	options->entries[options->count] = *option;
	options->count++;
}

ASTFXNode* nodeCreate(void* context, NodeType type)
{
	ASTFXNode* node = (ASTFXNode*)mmalloc(context, sizeof(ASTFXNode));
	node->options = nodeOptionsCreate(context);
	node->type = type;

	return node;
}

void nodeDelete(ASTFXNode* node)
{
	nodeOptionsDelete(node->options);

	mmfree(node);
}
