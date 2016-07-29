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

// borrowed from clib2 to be able to intermix C and C++ code with the AmigaOS3 g++ 2.95.3

#include <exec/types.h>
#include <proto/dos.h>
#include <stdlib.h>

#if defined(__amigaos4__)
static void (*__CTOR_LIST__[1]) (void) __attribute__(( used, section(".ctors"), aligned(sizeof(void (*)(void))) ));
static void (*__DTOR_LIST__[1]) (void) __attribute__(( used, section(".dtors"), aligned(sizeof(void (*)(void))) ));
#endif

// C-runtime specific constructor/destructor
// initialization routines.
#if defined(__MORPHOS__)
#include "libnix.c"
#include "constructors.h"
#endif

#if defined(__amigaos3__)
typedef void (*func_ptr)(void);

/****************************************************************************/

struct private_function
{
	func_ptr	function;
	int			priority;
};

/****************************************************************************/

extern func_ptr __INIT_LIST__[];
extern func_ptr __EXIT_LIST__[];

/****************************************************************************/

extern func_ptr __CTOR_LIST__[];
extern func_ptr __DTOR_LIST__[];

/****************************************************************************/

static void sort_private_functions(struct private_function * base, size_t count)
{
	struct private_function * a;
	struct private_function * b;
	size_t i,j;

	i = count - 2;

	do
	{
		a = base;

		for(j = 0 ; j <= i ; j++)
		{
			b = a + 1;

			if(a->priority > b->priority)
			{
				struct private_function t;

				t		= (*a);
				(*a)	= (*b);
				(*b)	= t;
			}

			a = b;
		}
	}
	while(i-- > 0);
}

/****************************************************************************/

/* Sort all the init and exit functions (private library constructors), then
   invoke the init functions in descending order. */
static void call_init_functions(void)
{
	LONG num_init_functions = (LONG)(__INIT_LIST__[0]) / 2;
	LONG num_exit_functions = (LONG)(__EXIT_LIST__[0]) / 2;

	if(num_init_functions > 1)
		sort_private_functions((struct private_function *)&__INIT_LIST__[1],num_init_functions);

	if(num_exit_functions > 1)
		sort_private_functions((struct private_function *)&__EXIT_LIST__[1],num_exit_functions);

	if(num_init_functions > 0)
	{
		struct private_function * t = (struct private_function *)&__INIT_LIST__[1];
		LONG i,j;

		for(j = 0 ; j < num_init_functions ; j++)
		{
			i = num_init_functions - (j + 1);

		  	(*t[i].function)();
		}
	}
}

/****************************************************************************/

/* Call all exit functions in ascending order; this assumes that the function
   table was prepared by call_init_functions() above. */
static void call_exit_functions(void)
{
	LONG num_exit_functions = (LONG)(__EXIT_LIST__[0]) / 2;

	if(num_exit_functions > 0)
	{
		STATIC LONG j = 0;

		struct private_function * t = (struct private_function *)&__EXIT_LIST__[1];
		LONG i;

		while(j++ < num_exit_functions)
		{
			i = j - 1;

			(*t[i].function)();
		}
	}
}

/****************************************************************************/

static void call_constructors(void)
{
	ULONG num_ctors = (ULONG)__CTOR_LIST__[0];
	ULONG i;

	/* Call all constructors in reverse order */
	for(i = 0 ; i < num_ctors ; i++)
	{
		__CTOR_LIST__[num_ctors - i]();
	}
}

/****************************************************************************/

static void call_destructors(void)
{
	ULONG num_dtors = (ULONG)__DTOR_LIST__[0];
	static ULONG i;

	/* Call all destructors in forward order */
	while(i++ < num_dtors)
	{
		__DTOR_LIST__[i]();
	}
}
#endif

void _init(void)
{
	#if defined(__amigaos4__)
	int num_ctors,i;
	int j;

	for(i = 1, num_ctors = 0 ; __CTOR_LIST__[i] != NULL ; i++)
		num_ctors++;

	for(j = 0 ; j < num_ctors ; j++)
		__CTOR_LIST__[num_ctors - j]();
	#elif defined(__amigaos3__)
	call_init_functions();
	call_constructors();
	#elif defined(__MORPHOS__)
	run_constructors();
	#endif
}

/****************************************************************************/

void _fini(void)
{
	#if defined(__amigaos4__)
	int num_dtors,i;
	static int j;

	for(i = 1, num_dtors = 0 ; __DTOR_LIST__[i] != NULL ; i++)
		num_dtors++;

	while(j++ < num_dtors)
		__DTOR_LIST__[j]();
	#elif defined(__amigaos3__)
	call_destructors();
	call_exit_functions();
	#elif defined(__MORPHOS__)
	run_destructors();
	#endif
}
