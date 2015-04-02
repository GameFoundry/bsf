#ifndef __ASTFX_H__
#define __ASTFX_H__

#include <stdlib.h>
#include <string.h>

typedef enum tagNodeType NodeType;
typedef enum tagOptionType OptionType;
typedef enum tagOptionDataType OptionDataType;
typedef struct tagOptionInfo OptionInfo;
typedef union tagOptionData OptionData;
typedef struct tagNodeOptions NodeOptions;
typedef struct tagNodeOption NodeOption;
typedef struct tagASTFXNode ASTFXNode;

enum tagNodeType
{
	NT_Shader,
	NT_Technique,
	NT_Parameters,
	NT_Blocks,
	NT_Pass
};

enum tagOptionType
{
	OT_Node,
	OT_Separable,
	OT_Priority,
	OT_Queue
	// TODO - Add others
};

enum tagOptionDataType
{
	ODT_Int,
	ODT_Float,
	ODT_Bool,
	ODT_String,
	ODT_Complex
};

struct tagOptionInfo
{
	OptionType type;
	OptionDataType dataType;
};

union tagOptionData
{
	int intValue;
	float floatValue;
	int boolValue;
	const char* strValue;
	void* complexValue;
};

struct tagNodeOption
{
	OptionType type;
	OptionData value;
};

struct tagNodeOptions
{
	NodeOption* entries;

	int count;
	int bufferSize;
};

struct tagASTFXNode
{
	NodeType type;
	NodeOptions* options;
	// TODO - Add parameters and blocks
};

OptionInfo OPTION_LOOKUP[];

NodeOptions* nodeOptionsCreate();
void nodeOptionDelete(NodeOption* option);
void nodeOptionsDelete(NodeOptions* options);
void nodeOptionsResize(NodeOptions* options, int size);
void nodeOptionsGrowIfNeeded(NodeOptions* options);
void nodeOptionsAdd(NodeOptions* options, NodeOption* option);

ASTFXNode* nodeCreate(NodeType type);
void nodeDelete(ASTFXNode* node);

#endif