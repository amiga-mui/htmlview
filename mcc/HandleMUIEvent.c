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

#include <math.h>
#include <clib/alib_protos.h>
#include <proto/commodities.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/keymap.h>
#include <proto/muimaster.h>

#include "General.h"
#include "Classes.h"
#include "Mark.h"
#include "SharedData.h"
#include "private.h"

extern struct SignalSemaphore mutex;

STRPTR FindTarget (Object *obj, STRPTR target, struct HTMLviewData *data)
{
	if(target)
	{
		if(!stricmp(target, "_self"))
			target = data->FrameName;
		else if(!stricmp(target, "_top"))
			target = data->Share->Data->FrameName;
		else if(!stricmp(target, "_parent"))
		{
			Object *parent = obj;
			
      if((parent = (Object *)xget(parent, MUIA_Parent)) &&
         (parent = (Object *)xget(parent, MUIA_Parent)))
      {
  	    target = (STRPTR)xget(parent, MUIA_HTMLview_FrameName);
      }
		}
	}
	else
	{
		target = data->FrameName;
	}
	return(target);
}
