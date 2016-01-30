//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#ifndef __INCLUDEHANDLER_H__
#define __INCLUDEHANDLER_H__

#include "BsASTFX.h"

#ifdef __cplusplus
extern "C" {
#endif

	char* includePush(ParseState* state, const char* filename, int line, int column, int* size);
	void includePop(ParseState* state);

#ifdef __cplusplus
}
#endif

#endif