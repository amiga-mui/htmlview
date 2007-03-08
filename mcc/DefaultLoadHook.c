/***************************************************************************

 HTMLview.mcc - HTMLview MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005 by TextEditor.mcc Open Source Team

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
#include <proto/dos.h>
#include <proto/exec.h>

//#include <clib/alib_protos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/commodities.h>
#include <proto/datatypes.h>

//#include <proto/asyncio.h>


#include "private.h"

/*
#define Open(file, a)         (BPTR)OpenAsync(file, MODE_READ, 2*4096)
#define Read(fh, buf, size)   ReadAsync((struct AsyncFile *)fh, buf, size)
#define Close(fh)             CloseAsync((struct AsyncFile *)fh)
*/

/*struct Library *AsyncIOBase;

VOID _INIT_2_AsyncIOBase ()
{
	if((AsyncIOBase = OpenLibrary("asyncio.library", 39L)) && AsyncIOBase->lib_Version == 39 && AsyncIOBase->lib_Revision < 2)
	{
		CloseLibrary(AsyncIOBase);
		AsyncIOBase = NULL;
	}
}

VOID _EXIT_2_AsyncIOBase ()
{
	if(AsyncIOBase)
		CloseLibrary(AsyncIOBase);
}*/
