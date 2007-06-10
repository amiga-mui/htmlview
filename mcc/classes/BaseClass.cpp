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

#include "BaseClass.h"
#include "HostClass.h"

#include "ParseMessage.h"
#include "ScanArgs.h"

VOID BaseClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
  AttrClass::Parse(pmsg);
  pmsg.SkipSpaces();

  struct ArgList args[] =
  {
    { "HREF",   &URL,     ARG_URL, NULL },
    { "TARGET", &Target,  ARG_URL, NULL },
    { NULL,     NULL,     0,       NULL }
  };
  ScanArgs(pmsg.Locked, args);

  if(URL)
  {
    ULONG last, i = last = 0;
    while(URL[i])
    {
      if(URL[i++] == '/')
        last = i;
    }
    URL[last] = '\0';
  }

  pmsg.Host->Base = this;
}
