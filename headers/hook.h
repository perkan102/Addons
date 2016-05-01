#pragma once

#include <dlfcn.h>
#include <cstring>
#include <sys/mman.h>

#include "Substrate.h"
// #include "offset.h"

#ifndef HOOKING

#define _GET(symbol) dlsym(RTLD_DEFAULT, symbol)
#define GET_SYMBOL(type, v_name, symbol) type v_name = (type)_GET(symbol)
#define C_HOOKING(ret, f_name, symbol) ret(*f_name)(...) = (ret(*)(...))_GET(symbol)
#define HOOKING(func, hook, real) ((void*)&func, (void*)&hook, (void**)&real)
#define HOOKING_SYMBOL(symbol, hook, real) (dlsym(RTLD_DEFAULT, symbol), (void*)&hook, (void**)&real)

#endif

typedef struct
{
	char name[128];
	const void* phdr;
	int phnum;
	unsigned entry;
	unsigned base;
	unsigned size;
} soinfo2;

void **GetVtable(const char *vtableSym);
int GetVtableIndex(void **vtable, const char *functionSym, int size);
void VirtualHook(void **vtable, int index, void *hook, void **real);
void VirtualHook(void **vtable, const char *functionSym, void *hook, void **real);
void tiny_hook(uint32_t*, uint32_t);
