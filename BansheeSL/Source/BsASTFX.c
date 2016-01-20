//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsASTFX.h"
#include "BsMMAlloc.h"

OptionInfo OPTION_LOOKUP[] =
{
	{ OT_None, ODT_Int },
	{ OT_Separable, ODT_Bool },
	{ OT_Priority, ODT_Int },
	{ OT_Queue, ODT_Int },
	{ OT_Transparent, ODT_Bool },
	{ OT_Technique, ODT_Complex }, 
	{ OT_Renderer, ODT_String }, 
	{ OT_Language, ODT_String }, 
	{ OT_Include, ODT_String }, 
	{ OT_Pass, ODT_Complex }, 
	{ OT_FillMode, ODT_Int }, 
	{ OT_CullMode, ODT_Int },
	{ OT_DepthBias, ODT_Float },
	{ OT_SDepthBias, ODT_Float },
	{ OT_DepthClip, ODT_Bool },
	{ OT_Scissor, ODT_Bool },
	{ OT_Multisample, ODT_Bool },
	{ OT_AALine, ODT_Bool },
	{ OT_DepthRead, ODT_Bool },
	{ OT_DepthWrite, ODT_Bool },
	{ OT_CompareFunc, ODT_Int },
	{ OT_Stencil, ODT_Bool },
	{ OT_StencilReadMask, ODT_Int },
	{ OT_StencilWriteMask, ODT_Int },
	{ OT_StencilOpFront, ODT_Complex },
	{ OT_StencilOpBack, ODT_Complex },
	{ OT_PassOp, ODT_Int },
	{ OT_Fail, ODT_Int },
	{ OT_ZFail, ODT_Int },
	{ OT_AlphaToCoverage, ODT_Bool },
	{ OT_IndependantBlend, ODT_Bool },
	{ OT_Target, ODT_Complex },
	{ OT_Index, ODT_Int },
	{ OT_Blend, ODT_Bool },
	{ OT_Color, ODT_Complex },
	{ OT_Alpha, ODT_Complex },
	{ OT_WriteMask, ODT_Int },
	{ OT_Source, ODT_Int },
	{ OT_Dest, ODT_Int },
	{ OT_Op, ODT_Int },
	{ OT_AddrMode, ODT_Complex },
	{ OT_MinFilter, ODT_Int },
	{ OT_MagFilter, ODT_Int },
	{ OT_MipFilter, ODT_Int },
	{ OT_MaxAniso, ODT_Int },
	{ OT_MipBias, ODT_Float },
	{ OT_MipMin, ODT_Float },
	{ OT_MipMax, ODT_Float },
	{ OT_BorderColor, ODT_Matrix },
	{ OT_U, ODT_Int },
	{ OT_V, ODT_Int },
	{ OT_W, ODT_Int },
	{ OT_Alias, ODT_String },
	{ OT_Auto, ODT_String },
	{ OT_Shared, ODT_Bool },
	{ OT_Usage, ODT_Int },
	{ OT_ParamType, ODT_Int },
	{ OT_Identifier, ODT_String },
	{ OT_ParamValue, ODT_Matrix },
	{ OT_ParamStrValue, ODT_String },
	{ OT_Parameters, ODT_Complex },
	{ OT_Blocks, ODT_Complex },
	{ OT_Parameter, ODT_Complex },
	{ OT_Block, ODT_Complex },
	{ OT_SamplerState, ODT_Complex },
	{ OT_Code, ODT_Complex },
	{ OT_StencilRef, ODT_Int }
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
		nodeDelete(option->value.nodePtr);
		option->value.nodePtr = 0;
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

	mmfree(originalEntries);
}

void nodeOptionsGrowIfNeeded(void* context, NodeOptions* options)
{
	static const int BUFFER_GROW = 10;

	if (options->count == options->bufferSize)
		nodeOptionsResize(context, options, options->bufferSize + BUFFER_GROW);
}

void nodeOptionsAdd(void* context, NodeOptions* options, const NodeOption* option)
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

void nodePush(ParseState* parseState, ASTFXNode* node)
{
	NodeLink* linkNode = (NodeLink*)mmalloc(parseState->memContext, sizeof(NodeLink));
	linkNode->next = parseState->nodeStack;
	linkNode->node = node;

	parseState->nodeStack = linkNode;
	parseState->topNode = node;
}

void nodePop(ParseState* parseState)
{
	if (!parseState->nodeStack)
		return;

	NodeLink* toRemove = parseState->nodeStack;
	parseState->nodeStack = toRemove->next;

	if (parseState->nodeStack)
		parseState->topNode = parseState->nodeStack->node;
	else
		parseState->topNode = 0;

	mmfree(toRemove);
}

ParseState* parseStateCreate()
{
	ParseState* parseState = (ParseState*)malloc(sizeof(ParseState));
	parseState->memContext = mmalloc_new_context();
	parseState->rootNode = nodeCreate(parseState->memContext, NT_Shader);
	parseState->topNode = 0;
	parseState->nodeStack = 0;

	parseState->hasError = 0;
	parseState->errorLine = 0;
	parseState->errorColumn = 0;
	parseState->errorMessage = 0;

	nodePush(parseState, parseState->rootNode);

	return parseState;
}

void parseStateDelete(ParseState* parseState)
{
	while (parseState->nodeStack != 0)
		nodePop(parseState);

	nodeDelete(parseState->rootNode);
	mmalloc_free_context(parseState->memContext);

	free(parseState);
}