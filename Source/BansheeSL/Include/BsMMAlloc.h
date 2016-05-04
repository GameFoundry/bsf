//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#ifndef __MMALLOC_H__
#define __MMALLOC_H__

void* mmalloc_new_context();
void mmalloc_free_context(void* context);
void* mmalloc(void* context, int size);
void mmfree(void* ptr);
char* mmalloc_strdup(void* context, const char* input);

#endif