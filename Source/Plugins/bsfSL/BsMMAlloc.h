//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#ifndef __MMALLOC_H__
#define __MMALLOC_H__

void* mmalloc_new_context();
void mmalloc_free_context(void* context);
void* mmalloc(void* context, int size);
void mmfree(void* ptr);
char* mmalloc_strdup(void* context, const char* input);

#endif
