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

 $Id: library.c 6 2007-03-12 07:33:29Z damato $

***************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <proto/dos.h>
#include <proto/exec.h>

// change the libnix pool size by defining _MSTEP var

int ThisRequiresConstructorHandling;
static VOID *mempool;

extern const unsigned long	__ctrslist[];
extern const unsigned long	__dtrslist[];
extern const struct CTDT __ctdtlist[];

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

static int constructors_done;
static struct CTDT *last_ctdt;

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
			return 0;
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
}

void *malloc(size_t size)
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
	}
}

void __chkabort(void) { }
void abort(void) { }
void exit(int err) { }

/*
 * .pctors,.pdtors instead of .init,.fini
 * as the later are std *CODE* sections.
 */

asm("\n.section \".ctors\",\"a\",@progbits\n__ctrslist:\n.long -1\n");
asm("\n.section \".dtors\",\"a\",@progbits\n__dtrslist:\n.long -1\n");
__asm("\n.section \".ctdt\",\"a\",@progbits\n__ctdtlist:\n.long -1,-1\n");
