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

#include "Debug.h"
#include "General.h"

APTR operator new(size_t bytes) throw()
{
  APTR res;

  for (;;)
  {
    res = malloc(bytes);
    if (res) break;
    Delay(10);
  }

  return res;
}

void* operator new[](size_t t) throw ()
{
  return operator new(t);
}

void* operator new(size_t t,void *p) throw ()
{
  return p;
}

VOID operator delete[] (APTR mem)
{
  return operator delete(mem);
}

VOID operator delete (APTR mem)
{
  if (mem) free(mem);
}

extern "C" VOID _INIT_4_InitMem ();
VOID _INIT_4_InitMem ()
{
}

extern "C" VOID _EXIT_4_CleanupMem();
VOID _EXIT_4_CleanupMem()
{
}

#ifndef __MORPHOS__
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
#endif

