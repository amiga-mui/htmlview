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

#include "BrClass.h"

#include "Layout.h"
#include "MinMax.h"
#include "ParseMessage.h"
#include "ScanArgs.h"

BOOL BrClass::Layout (struct LayoutMessage &lmsg)
{
  if(lmsg.Baseline + lmsg.Bottom == 0)
    lmsg.UpdateBaseline(lmsg.Font->tf_YSize+1, lmsg.Font->tf_Baseline);

  lmsg.Newline();
  lmsg.FlushImages(Clear);

  return AttrClass::Layout(lmsg);
}

BOOL BrClass::Mark (struct MarkMessage &mmsg)
{
  BOOL result = SuperClass::Mark(mmsg);

  mmsg.Newline = TRUE;
  if(mmsg.Enabled())
    mmsg.WriteLF();

  return(result);
}

VOID BrClass::MinMax (struct MinMaxMessage &mmsg)
{
  mmsg.Newline();

  Flags |= FLG_KnowsMinMax;
}

VOID BrClass::Parse(struct ParseMessage &pmsg)
{
  AttrClass::Parse(pmsg);
  pmsg.SkipSpaces();

  const char *ClearKeywords[] = { "LEFT", "RIGHT", "ALL", NULL };
  LONG clear = -1;
  struct ArgList args[] =
  {
    { "CLEAR",  &clear, ARG_KEYWORD, ClearKeywords },
    { NULL,     NULL,   0,           NULL          }
  };
  ScanArgs(pmsg.Locked, args);
  Clear = clear+1;
}

