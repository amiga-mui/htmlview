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

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <exec/memory.h>
#include <exec/tasks.h>
#include <proto/dos.h>
#include <proto/exec.h>

#include "Debug.h"
#include "General.h"

//#define MEMWATCH
#define THREAD_SAFE
#define POOL_SIZE (31*1024)

#ifndef PROGRAM
#define PROGRAM FindTask(NULL)->tc_Node.ln_Name
#endif

#ifdef THREAD_SAFE
struct SignalSemaphore MemorySemaphore;
#endif

APTR MemoryPool = NULL;

struct Pool
{
  VOID Init (ULONG size)
  {
//    Allocated = Freed = 0;
    PoolSize = size;
  }

  ULONG Allocated;
  ULONG Freed;
  ULONG PoolSize;
  UBYTE Data[POOL_SIZE];
};

struct Memory
{
  struct Pool *SrcPool;
  ULONG Size;
//  UBYTE Tag[8];
  UBYTE DataStart;
};

struct Pool *CurrentPool;

inline struct Pool* NewPool(ULONG size)
{
  ENTER();

  struct Pool *result;
  do
  {
    result = (Pool*)AllocPooled(MemoryPool, size);
  }
  while(!result);
  
  result->Init(size);

  RETURN(result);
  return(result);
}

APTR operator new(size_t bytes)
{
  ENTER();

  bytes = (offsetof(Memory, DataStart)+bytes+8) & ~7;

#ifdef THREAD_SAFE
  ObtainSemaphore(&MemorySemaphore);
#endif

  struct Pool *pool = CurrentPool;
  if(bytes > POOL_SIZE)
  {
    ULONG pool_size = offsetof(Pool, Data)+bytes;
    pool = NewPool(pool_size);
  }
  else if(CurrentPool->Allocated + bytes > POOL_SIZE)
  {
    pool = NewPool(sizeof(Pool));
    CurrentPool = pool;
  }

  struct Memory *mem = (struct Memory *)&pool->Data[pool->Allocated];
  pool->Allocated += bytes;

#ifdef THREAD_SAFE
  ReleaseSemaphore(&MemorySemaphore);
#endif

  mem->SrcPool = pool;
  mem->Size = bytes;
//  strncpy(&mem->Tag[0], "C0DEDBAD", 8);

  RETURN(&mem->DataStart);
  return(&mem->DataStart);
}

VOID operator delete (APTR mem)
{
  if(mem)
  {
    struct Memory *chunk = (struct Memory *)((ULONG)mem - offsetof(Memory, DataStart));
    struct Pool *pool = chunk->SrcPool;

/*    if(strncmp(chunk->Tag, "C0DEDBAD", 8))
      printf("*** broken: %s\n", &chunk->DataStart);
*/
#ifdef THREAD_SAFE
    ObtainSemaphore(&MemorySemaphore);
#endif
    pool->Freed += chunk->Size;
    if(pool->Freed == pool->Allocated)
    {
      if(pool != CurrentPool)
      {
        FreePooled(MemoryPool, pool, pool->PoolSize);
      }
      else
      {
        pool->Allocated = pool->Freed = 0;
        memset(pool->Data, 0, pool->PoolSize - offsetof(Pool, Data));
      }
    }
#ifdef THREAD_SAFE
    ReleaseSemaphore(&MemorySemaphore);
#endif
  }
}

extern "C" VOID _INIT_4_InitMem ();
VOID _INIT_4_InitMem ()
{
#ifdef THREAD_SAFE
  memset(&MemorySemaphore, 0, sizeof(struct SignalSemaphore));
  InitSemaphore(&MemorySemaphore);
#endif

  #if defined(__amigaos4__)
  MemoryPool = IExec->AllocSysObjectTags(ASOT_MEMPOOL,
                                         ASOPOOL_MFlags,     MEMF_CLEAR | MEMF_SHARED,
                                         ASOPOOL_Puddle,     32*1024,
                                         ASOPOOL_Threshold,  8*1024,
                                         ASOPOOL_Protected,  TRUE,
                                         ASOPOOL_Name,       "HTMLview.mcc",
                                         TAG_DONE);
  #else
  MemoryPool = CreatePool(MEMF_CLEAR | MEMF_ANY, 32*1024, 8*1024);
  #endif

  CurrentPool = (Pool *)AllocPooled(MemoryPool, sizeof(Pool));
  CurrentPool->Init(sizeof(Pool));
}

extern "C" VOID _EXIT_4_CleanupMem ();
VOID _EXIT_4_CleanupMem ()
{
  if(MemoryPool)
  {
    FreePooled(MemoryPool, CurrentPool, CurrentPool->PoolSize);
    #if defined(__amigaos4__)
    FreeSysObject(ASOT_MEMPOOL, MemoryPool);
    #else
    DeletePool(MemoryPool);
    #endif
  }
}

void *malloc(size_t size)
{
   ULONG *mem = (ULONG *)AllocVecPooled(MemoryPool, size + 4);
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
      size_t min_size = min(old_size, size);
      memcpy(result, ptr, min_size);
   }

   free(ptr);

   return result;
}

