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

 $Id: rev.h,v 1.4 2005/04/11 03:04:38 tactica Exp $

***************************************************************************/

#include <stdlib.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <string.h>

int ThisRequiresConstructorHandling = 0;
VOID *mempool = NULL;

struct CTDT
{
	int	(*fp)(void);
	long	priority;
};

struct HunkSegment
{
	unsigned int Size;
	struct HunkSegment *Next;
};

extern const unsigned long	__ctrslist[];
extern const unsigned long	__dtrslist[];
extern const struct CTDT __ctdtlist[];

static void __HandleConstructors(void (*FuncArray[])(void))
{
	struct HunkSegment *MySegment;
	LONG i, j;

	MySegment = (struct HunkSegment*) (( (unsigned int) FuncArray) - sizeof(struct HunkSegment));

	i = 0;
	j = (MySegment->Size - sizeof(struct HunkSegment)) / sizeof(void *);

	while ((i < j) && (FuncArray[i]))
	{
		/* skip first entry..trick to avoid a 0 size section->boom
		 */

		if (FuncArray[i] != ((void (*)(void)) -1))
		{
			(*FuncArray[i])();
		}
		i++;
	}
}

static int comp_ctdt(struct CTDT *a, struct CTDT *b)
{
	if (a->priority == b->priority)
		return (0);
	if ((unsigned long)a->priority < (unsigned long) b->priority)
		return (-1);

	return (1);
}

static int constructors_done = 0;
static struct CTDT *last_ctdt = NULL;

ULONG run_constructors(void)
{
	static int sorted = 0;
	struct CTDT *ctdt = (struct CTDT *)__ctdtlist;

	if (constructors_done)
		return 1;

	if (!mempool)
	{
		mempool = CreatePool(MEMF_CLEAR | MEMF_SEM_PROTECTED, 12*1024, 6*1024);

		if (!mempool)
        {
			return 0;
        }
	}

	if (!sorted)
	{
		struct HunkSegment *seg;

		sorted = 1;
		seg = (struct HunkSegment *)(((unsigned int)ctdt) - sizeof(struct HunkSegment));
		last_ctdt = (struct CTDT *)(((unsigned int)seg) + seg->Size);

		qsort((struct CTDT *)ctdt, last_ctdt - ctdt, sizeof(*ctdt), (int (*)(const void *, const void *))comp_ctdt);
	}

	while (ctdt < last_ctdt)
	{
		if (ctdt->priority >= 0)
		{
			if(ctdt->fp() != 0)
			{
				return 0;
			}
		}
		ctdt++;
	}

	//malloc(0);

	__HandleConstructors((void (**)(void)) __ctrslist);

	constructors_done = 1;

    return 1;
}

VOID run_destructors(void)
{
	const struct CTDT *ctdt = __ctdtlist;

	if (constructors_done)
	{
		__HandleConstructors((void (**)(void)) __dtrslist);
	}

	while (ctdt < last_ctdt)
	{
		if (ctdt->priority < 0)
		{
			if(ctdt->fp != (int (*)(void)) -1)
			{
				ctdt->fp();
			}
		}
		ctdt++;
	}

	if (mempool)
	{
		DeletePool(mempool);
		mempool = NULL;
	}

	constructors_done = 0;
}

//ULONG totmem = 0;

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

void free(void *p)
{
    //totmem-=*((ULONG *)p-1);
    //kprintf("MEM FREE %lx %ld\n",p,totmem);

    if (p && mempool)
    	FreePooled(mempool,(ULONG *)p-1,*((ULONG *)p-1));
}

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

/*void *malloc(size_t size)
{
	ULONG *p = NULL;

	if (size)
	{
		size += 4;

		//p = (ULONG *)AllocPooledAligned(mempool, size, 8, 4);
		p = (ULONG *)AllocPooled(mempool, size);

		if (p)
		{
			*p++ = size;
		}

  totmem+=size;
  kprintf("MEM MALLOC %lx %ld\n",p,totmem);
	}

	return (void *)p;
}

void free(void *p)
{
	if (p && mempool)
	{
		ULONG size, *ptr = (ULONG *)p;
		size = *--ptr;
		FreePooled(mempool, ptr, size);

  totmem-=size;
  kprintf("MEM FREE %lx %ld\n",p,totmem);
	}
}

#ifndef min
#define min(a,b) (a<b?a:b)
#endif

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
}*/

void __chkabort(void) { }
void abort(void) { Wait(0);}
void exit(int UNUSED err) { Wait(0);}

/*
 * .pctors,.pdtors instead of .init,.fini
 * as the later are std *CODE* sections.
 */

/*int
sprintf(STRPTR buf,STRPTR fmt,...)
{
    va_list va;

    va_start(va,fmt);
    VNewRawDoFmt(fmt,(void* (*)(void*, UBYTE))0,buf,va);
    va_end(va);
}*/

asm("\n.section \".ctors\",\"a\",@progbits\n__ctrslist:\n.long -1\n");
asm("\n.section \".dtors\",\"a\",@progbits\n__dtrslist:\n.long -1\n");
__asm("\n.section \".ctdt\",\"a\",@progbits\n__ctdtlist:\n.long -1,-1\n");
