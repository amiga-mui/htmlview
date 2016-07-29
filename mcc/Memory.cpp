/***************************************************************************

 HTMLview.mcc - HTMLview MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005-2007 by HTMLview.mcc Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 HTMLview class Support Site:  http://www.sf.net/projects/htmlview-mcc/

 $Id$

***************************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <exec/memory.h>
#include <exec/tasks.h>
#include <proto/dos.h>
#include <proto/exec.h>

#include <clib/macros.h>

#include "Debug.h"
#include "General.h"
#include <new>
#include "private.h"

//#define MEMWATCH
#define THREAD_SAFE
#define POOL_SIZE (31*1024)

#ifndef PROGRAM
#define PROGRAM FindTask(NULL)->tc_Node.ln_Name
#endif

#ifdef __MORPHOS__
extern APTR mempool;
#define MemoryPool mempool
#else
APTR MemoryPool = NULL;
#endif

#if defined(__amigaos3__)
struct SignalSemaphore MemoryPoolSema;
#endif

#if defined(__MORPHOS__)
/*******************************************************************/

/*#define NewRawDoFmt(__p0, __p1, __p2, ...) \
	(((STRPTR (*)(void *, CONST_STRPTR , APTR (*)(APTR, UBYTE), STRPTR , ...))*(void**)((long)(EXEC_BASE_NAME) - 922))((void*)(EXEC_BASE_NAME), __p0, __p1, __p2, __VA_ARGS__))*/

//extern struct SignalSemaphore ttt;
//extern ULONG tttfail;

/*******************************************************************/

void *malloc(size_t size)
{
    if (size && mempool)
    {
	    ULONG *mem;

    	mem = (ULONG *)AllocPooled(mempool,size = size+sizeof(ULONG));
        if (mem) *mem++ = size;

  //totmem+=size;
  //kprintf("MEM MALLOC %lx %ld\n",mem,totmem);

    	return mem;
	}

	return NULL;
}

/*******************************************************************/

void free(void *p)
{
    //totmem-=*((ULONG *)p-1);
    //kprintf("MEM FREE %lx %ld\n",p,totmem);

    if (p && mempool)
    	FreePooled(mempool,(ULONG *)p-1,*((ULONG *)p-1));
}

/*******************************************************************/

void *realloc(void *mem, size_t size)
{
    APTR  nmem;
    ULONG sold = 0; //gcc

    if (size==0) return NULL;

    if (mem)
    {
        sold = *((ULONG *)mem-1);

        if (sold-sizeof(ULONG)>=size) return mem;
    }

    nmem = malloc(size);
    if (nmem)
    {
        if (mem) memcpy(nmem,mem,sold);
    }

    if (mem) free(mem);

    return nmem;
}

#endif

/*******************************************************************/

//#define CANTFAILNONONO

#ifdef CANTFAILNONONO

void *cantFailMalloc(size_t size)
{
    for (;;)
    {
        APTR res = malloc(size);
        if (res) return res;
        Delay(5);
	}
}
#define _MALLOC cantFailMalloc
#else
#define _MALLOC malloc
#endif

/*******************************************************************/

APTR operator new(std::size_t bytes,const std::nothrow_t&) throw()
{
/*  {
    ULONG f;
	ObtainSemaphore(&ttt);
	f = tttfail;
	ReleaseSemaphore(&ttt);
    if (f)
    {
	  //NewRawDoFmt("!!!!!!!!!!!!!!!!......New std compelled to fail on %ld\n",(void * (*)(void *, UBYTE))1,NULL,bytes);
      return 0;
    }
  }*/

  //NewRawDoFmt("......New std %ld\n",(void * (*)(void *, UBYTE))1,NULL,bytes);
  return _MALLOC(bytes);
}

/*******************************************************************/

void *operator new[](std::size_t bytes, const std::nothrow_t&) throw()
{
  /*{
    ULONG f;
	ObtainSemaphore(&ttt);
	f = tttfail;
	ReleaseSemaphore(&ttt);
    if (f)
    {
	  NewRawDoFmt("!!!!!!!!!!!!!!!!......New[] std compelled to fail on %ld\n",(void * (*)(void *, UBYTE))1,NULL,bytes);
      return 0;
    }
  }*/

  //NewRawDoFmt("......New[] %ld\n",(void * (*)(void *, UBYTE))1,NULL,bytes);
  return _MALLOC(bytes);
}

/*******************************************************************/

APTR operator new(size_t bytes) throw()
{
  //NewRawDoFmt("......!!! New %ld\n",(void * (*)(void *, UBYTE))1,NULL,bytes);
  return _MALLOC(bytes);
}

void* operator new[](size_t t) throw ()
{
  //NewRawDoFmt("......!!! New[] %ld\n",(void * (*)(void *, UBYTE))1,NULL,t);
  return operator new(t);

}

/*******************************************************************/

/*void* operator new(size_t t,void *p) throw ()
{
    NewRawDoFmt("......NewP %lx\n",(void * (*)(void *, UBYTE))1,NULL,p);
	return p;
}*/

VOID operator delete[] (APTR mem)
{
    //NewRawDoFmt("......Delete[] %lx\n",(void * (*)(void *, UBYTE))1,NULL,mem);
    return operator delete(mem);
}

/*******************************************************************/

VOID operator delete (APTR mem)
{
  //NewRawDoFmt("......Delete %lx\n",(void * (*)(void *, UBYTE))1,NULL,mem);
  if (mem) free(mem);
}

/*******************************************************************/

#if defined(__amigaos3__)
APTR AllocVecPooled(APTR pool, ULONG size)
{
	ULONG *mem;

	size += sizeof(*mem);

	ObtainSemaphore(&MemoryPoolSema);
	if((mem = (ULONG *)AllocPooled(pool, size)) != NULL)
		*mem++ = size;
	ReleaseSemaphore(&MemoryPoolSema);

	return (APTR)mem;
}

void FreeVecPooled(APTR pool, APTR mem)
{
	if(mem != NULL)
	{
		ULONG *p, size;

		p = (ULONG *)mem;
		size = *--p;

		ObtainSemaphore(&MemoryPoolSema);
		FreePooled(pool, p, size);
		ReleaseSemaphore(&MemoryPoolSema);
	}
}
#endif

CONSTRUCTOR(InitMem, 10)
{
  #ifndef __MORPHOS__
	  #if defined(__amigaos4__)
	  MemoryPool = AllocSysObjectTags(ASOT_MEMPOOL,
                                      ASOPOOL_MFlags,     MEMF_CLEAR | MEMF_SHARED,
                                      ASOPOOL_Puddle,     32*1024,
                                      ASOPOOL_Threshold,  8*1024,
                                      ASOPOOL_Protected,  TRUE,
                                      ASOPOOL_Name,       "HTMLview.mcc",
                                      TAG_DONE);
	  #elif defined(__amigaos3__)
	  InitSemaphore(&MemoryPoolSema);
	  MemoryPool = CreatePool(MEMF_CLEAR | MEMF_ANY, 32*1024, 8*1024);
	  #else
	  MemoryPool = CreatePool(MEMF_CLEAR | MEMF_ANY | MEMF_SEM_PROTECTED, 32*1024, 8*1024);
	  #endif
  #endif
}

DESTRUCTOR(CleanupMem, 10)
{
  if(MemoryPool)
  {

	#ifndef __MORPHOS__
	    #if defined(__amigaos4__)
	    FreeSysObject(ASOT_MEMPOOL, MemoryPool);
    	#else
	    DeletePool(MemoryPool);
    	MemoryPool = NULL;
	    #endif
    #endif
  }
}

/*******************************************************************/

#ifndef __MORPHOS__
void *malloc(size_t size)
{
   ULONG *mem;
   mem = (ULONG *)AllocVecPooled(MemoryPool, size + 4);
   if (mem)
   {
      *mem++ = size;
   }

   return mem;
}

void free(void *ptr)
{
   ULONG *mem = (ULONG *)ptr;
   if (mem)
   {
     --mem;
     FreeVecPooled(MemoryPool, mem);
   }
}

void *realloc(void *ptr, size_t size)
{
   size_t old_size = 0;
   void *result;

   if (ptr)
   {
      ULONG *mem = (ULONG *)ptr;
      old_size = *--mem;
   }

   result = malloc(size);
   if (result && ptr)
   {
      size_t min_size = MIN(old_size, size);
      memcpy(result, ptr, min_size);
   }

   free(ptr);

   return result;
}
#endif

/*******************************************************************/

