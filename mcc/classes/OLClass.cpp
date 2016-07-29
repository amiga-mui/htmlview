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

#include "OLClass.h"

#include "Layout.h"
#include "ParseMessage.h"
#include "ScanArgs.h"

BOOL OLClass::Layout (struct LayoutMessage &lmsg)
{
  UWORD old_type = lmsg.OL_Type;
  lmsg.OL_Type = Type;
  ULClass::Layout(lmsg);
  lmsg.OL_Type = old_type;

   return TRUE;
}

BOOL OLClass::Mark (struct MarkMessage &mmsg)
{
  UWORD old_type = mmsg.OL_Type;
  mmsg.OL_Type = Type;
  ULClass::Mark(mmsg);
  mmsg.OL_Type = old_type;

   return TRUE;
}

VOID OLClass::Parse(struct ParseMessage &pmsg)
{
  pmsg.SetLock();
  pmsg.NextEndBracket();
#ifdef OUTPUT
  PrintTag(pmsg.Locked);
#endif

  STRPTR type = NULL;
  LONG start = 1;
  struct ArgList args[] =
  {
    { "TYPE", &type,    ARG_STRING, NULL  },
    { "START",  &start,   ARG_NUMBER, NULL  },
    { NULL,     NULL,       0,          NULL  }
  };
  ScanArgs(pmsg.Locked, args);

  if(type)
  {
    switch(*type)
    {
      case 'a':
        Type = OL_LowerAlpha;
      break;

      case 'A':
        Type = OL_UpperAlpha;
      break;

      case 'i':
        Type = OL_LowerRoman;
      break;

      case 'I':
        Type = OL_UpperRoman;
      break;
    }
    delete type;
  }

  APTR handle;
  if((handle = Backup(pmsg, 1, tag_LI)))
  {
    if(++pmsg.OL_Level < 3)
      pmsg.OL_Cnt[pmsg.OL_Level] = start;
    TreeClass::Parse(pmsg);
    pmsg.OL_Level--;
    Restore(pmsg.OpenCounts, 1, handle);
  }
}

VOID OLClass::Render (struct RenderMessage &rmsg)
{
  UWORD old_type = rmsg.OL_Type;
  rmsg.OL_Type = Type;
  ULClass::Render(rmsg);
  rmsg.OL_Type = old_type;
}

