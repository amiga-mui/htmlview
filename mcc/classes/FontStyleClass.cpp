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

#include "FontStyleClass.h"

#include "Layout.h"
#include "MinMax.h"
#include "ParseMessage.h"
#include "ScanArgs.h"
#include "SharedData.h"

FontStyleClass::FontStyleClass (UBYTE textstyles, UBYTE alignment, BOOL newline, BYTE font) : TreeClass()
{
  Textstyles = textstyles;
  Alignment = alignment;
  Font = font;
  if(newline)
    Flags |= FLG_Newline;
}

BOOL FontStyleClass::Layout (struct LayoutMessage &lmsg)
{
  if((Flags & (FLG_Virgin | FLG_Newline)) == (FLG_Virgin | FLG_Newline))
  {
    lmsg.EnsureNewline();
    if(lmsg.MinY < lmsg.Y)
    {
      if(ID == tag_PRE)
        lmsg.AddYSpace(5);
      else if(Font >= Font_H1 && Font <= Font_H6)
        lmsg.AddYSpace(lmsg.Share->Fonts[Font]->tf_YSize);
    }
  }

  UBYTE oldalign = lmsg.Align;
  if(Alignment)
    lmsg.Align = Alignment;

  struct TextFont *oldfont = lmsg.Font;
  if(Font != Font_None)
    lmsg.Font = lmsg.Share->Fonts[Font];

  if(TreeClass::Layout(lmsg))
  {
    /* This may add extra space, in situations where it isn't needed */
    if(Textstyles & TSF_Italic)
      lmsg.X += lmsg.Font->tf_Baseline/2;

    if(Flags & FLG_Newline)
      lmsg.EnsureNewline();

    if(ID == tag_PRE)
      lmsg.AddYSpace(5);
  }

  lmsg.Font = oldfont;
  lmsg.Align = oldalign;

  return TRUE;
}

VOID FontStyleClass::MinMax (struct MinMaxMessage &mmsg)
{
  if(Flags & FLG_Newline)
    mmsg.Newline();

  struct TextFont *oldfont = mmsg.Font;
  if(Font != Font_None)
    mmsg.Font = mmsg.Fonts[Font];

  TreeClass::MinMax(mmsg);

  if(Textstyles & TSF_Italic)
    mmsg.X += mmsg.Font->tf_Baseline/2;

  mmsg.Font = oldfont;
}

VOID FontStyleClass::Parse(struct ParseMessage &pmsg)
{
  pmsg.SetLock();
  pmsg.NextEndBracket();
#ifdef OUTPUT
  PrintTag(pmsg.Locked);
#endif

  if(Alignment == Align_Query)
  {
    LONG alignment = -1;
    struct ArgList args[] =
    {
      { "ALIGN",  &alignment, ARG_KEYWORD, AlignKeywords },
      { NULL,     NULL,       0,           NULL          }
    };
    ScanArgs(pmsg.Locked, args);
    Alignment = alignment+1;
  }
  TreeClass::Parse(pmsg);
}

WORD FontStyleClass::InDomain (struct RenderMessage &rmsg)
{
  return((Flags & FLG_Newline) ? SuperClass::InDomain(rmsg) : (WORD)InsideLayer);
}

VOID FontStyleClass::Render (struct RenderMessage &rmsg)
{
  UBYTE oldtextstyles = rmsg.Textstyles;
  rmsg.Textstyles |= Textstyles;
  TreeClass::Render(rmsg);
  rmsg.Textstyles = oldtextstyles;
}

