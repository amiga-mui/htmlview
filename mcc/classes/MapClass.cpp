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

#include "MapClass.h"

#include "Map.h"
#include "ParseMessage.h"
#include "ScanArgs.h"

class MapClass *MapClass::FindMap (STRPTR name)
{
  return(stricmp(Name, name+1) ? TreeClass::FindMap(name) : this);
}

BOOL MapClass::UseMap (struct UseMapMessage &umsg)
{
  return(TreeClass::UseMap(umsg) && umsg.URL);
}

VOID MapClass::Parse(struct ParseMessage &pmsg)
{
  pmsg.SetLock();
  pmsg.NextEndBracket();
#ifdef OUTPUT
  PrintTag(pmsg.Locked);
#endif

  struct ArgList args[] =
  {
    { "NAME", &Name,  ARG_URL, NULL },
    { NULL,     NULL,    0,       NULL }
  };
  ScanArgs(pmsg.Locked, args);

  TreeClass::Parse(pmsg);
}

