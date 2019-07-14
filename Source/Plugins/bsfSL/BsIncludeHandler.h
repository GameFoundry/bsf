//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#ifndef __INCLUDEHANDLER_H__
#define __INCLUDEHANDLER_H__

#include "BsASTFX.h"

char* includePush(ParseState* state, const char* filename, int line, int column, int* size);
void includePop(ParseState* state);

#endif
