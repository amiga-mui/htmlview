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
#include <clib/alib_protos.h>
#include <proto/muimaster.h>
#include <libraries/mui.h>
#include <mui/BetterString_mcc.h>
#include <mui/TextEditor_mcc.h>

#include "General.h"
#include "Classes.h"
#include "private.h"
#include "ScrollGroup.h"
#include "Forms.h"
#include "SharedData.h"

/*
FILE *outFile = NULL;
VOID _INIT_6_OpenIO ()
{
	outFile = fopen("CON://///AUTO/WAIT/CLOSE", "r+") ;
}
VOID _EXIT_6_CloseIO ()
{
	fclose(outFile);
}

VOID DumpClass (struct IClass &ic)
{
	fprintf(outFile,
		"Dispatcher:    %p, %p, %p\n"
		"Reserved:      %p\n"
		"Super:         %p\n"
		"ID:            %p\n"
		"InstOffset:    %p\n"
		"InstSize:      %p\n"
		"UserData:      %p\n"
		"SubclassCount: %d\n"
		"ObjectCount:   %d\n"
		"Flags:         %p\n\n",
		ic.cl_Dispatcher.h_Entry, ic.cl_Dispatcher.h_SubEntry, ic.cl_Dispatcher.h_Data,
		ic.cl_Reserved, ic.cl_Super, ic.cl_ID, ic.cl_InstOffset, ic.cl_InstSize, ic.cl_UserData, ic.cl_SubclassCount, ic.cl_ObjectCount, ic.cl_Flags);
}
BOOL first = TRUE;
*/

