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

#include "LIClass.h"

#include "Layout.h"
#include "MinMax.h"
#include "ParseMessage.h"
#include "ScanArgs.h"
#include "SharedData.h"

#include <ctype.h>

VOID LIClass::NumToStr (STRPTR str, UWORD type)
{
  BOOL lower = FALSE;
  switch(type)
  {
    case OL_LowerRoman:
      lower = TRUE;
    case OL_UpperRoman:
      DecToRoman(Number, str);
    break;

    case OL_LowerAlpha:
      lower = TRUE;
    case OL_UpperAlpha:
    {
      char num = Number + 'A' - 1;
      
         while(num > 'Z')
        num -= 'Z' - 'A' + 1;

      sprintf(str, "%c.", num);
    }
    break;

    default:
      sprintf(str, "%ld.", Number);
    break;
  }

  if(lower)
  {
    STRPTR t_str = str;
    while(*t_str)
      {
      *t_str = tolower(*t_str);
         t_str++;
      }
  }
}

BOOL LIClass::Layout (struct LayoutMessage &lmsg)
{
  ULONG width = lmsg.LIIndent;
  if(Flags & FLG_Virgin)
  {
    lmsg.EnsureNewline();

    if(Number)
    {
      char str[64];
      NumToStr(str, lmsg.OL_Type);
      struct TextFont *font = lmsg.Share->Fonts[Font_Normal];

      ULONG needed_width = MyTextLength(font, str, strlen(str));
      if(needed_width > width+20)
        width = lmsg.LIIndent = needed_width-20;

      lmsg.UpdateBaseline(font->tf_YSize+1, font->tf_Baseline);
    }
    else
    {
      LONG height = lmsg.Share->LI_Height;
      lmsg.UpdateBaseline(height+1, height-1);
    }

    lmsg.MinY = lmsg.Y; // This indicates that no additional linebreaks should be placed infront of P-tags etc.

    lmsg.X += width+4;
    lmsg.Indent += width+4;
    Left = lmsg.X-4;

    struct ObjectNotify *notify = new struct ObjectNotify(Left, Baseline, NULL);
    lmsg.AddNotify(notify);
  }

  if(TreeClass::Layout(lmsg))
  {
    lmsg.EnsureNewline();
    if(lmsg.MinY == lmsg.Y)
      lmsg.Newline();
    lmsg.X -= width+4;
    lmsg.Indent -= width+4;

    lmsg.TopChange = min(lmsg.TopChange, Top);
  }

   return TRUE;
}

VOID LIClass::AdjustPosition (LONG x, LONG y)
{
  Left += x;
  Baseline += y;
  TreeClass::AdjustPosition(x, y);
}

BOOL LIClass::Mark (struct MarkMessage &mmsg)
{
  if(mmsg.Enabled())
  {
    if(Number)
    {
      char str[64];

      NumToStr(str, mmsg.OL_Type);
      mmsg.WriteText(str, strlen(str));
      mmsg.WriteText((STRPTR)" ", 1);
    }
    else
      mmsg.WriteText((STRPTR)"o ", 2);

    mmsg.Newline = TRUE;
  }

  return(TreeClass::Mark(mmsg));
}

VOID LIClass::MinMax (struct MinMaxMessage &mmsg)
{
  mmsg.Indent += 14;
  TreeClass::MinMax(mmsg);
  mmsg.X -= 14;
  mmsg.Indent -= 14;
}

VOID LIClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
  pmsg.SetLock();
  pmsg.NextEndBracket();
#ifdef OUTPUT
  PrintTag(pmsg.Locked);
#endif

  LONG number = pmsg.OL_Cnt[pmsg.OL_Level];
  struct ArgList args[] =
  {
    { "VALUE",  &number,    ARG_NUMBER, NULL  },
    { NULL,     NULL,       0,          NULL  }
  };
  ScanArgs(pmsg.Locked, args);

  if(pmsg.OpenCounts[tag_OL])
  {
    if(pmsg.OL_Level < 3)
    {
      Number = number++;
      pmsg.OL_Cnt[pmsg.OL_Level] = number;
    }
  }
  TreeClass::Parse(pmsg);
}

VOID LIClass::Render (struct RenderMessage &rmsg)
{
  if(Flags & FLG_Layouted)
  {
    if(!rmsg.TargetObj)
    {
      struct RastPort *rp = rmsg.RPort;
      if(Number)
      {
        char str[64];
        NumToStr(str, rmsg.OL_Type);
        struct TextFont *font = rmsg.Share->Fonts[Font_Normal];
        ULONG len = strlen(str);
        ULONG x = Left - MyTextLength(font, str, len) - rmsg.OffsetX;
        SetAPen(rp, rmsg.Colours[Col_Text]);
        SetFont(rp, font);
        Move(rp, x, Baseline - rmsg.OffsetY);
        Text(rp, str, len);
      }
      else
      {
        if(rmsg.Share->LI_BMp)
        {
          LONG x, y, width = rmsg.Share->LI_Width, height = rmsg.Share->LI_Height;
          x = Left - width - rmsg.OffsetX;
          y = Baseline - rmsg.OffsetY - height + 1;

          ULONG index = 0;
          if(rmsg.UL_Nesting > 1)
            index = width * min(rmsg.UL_Nesting-1, 2);

          if(rmsg.Share->LI_Mask)
              BltMaskRPort(rmsg.Share->LI_BMp, index, 0, rp, x, y, width, height, rmsg.Share->LI_Mask);
          else  BltBitMapRastPort(rmsg.Share->LI_BMp, index, 0, rp, x, y, width, height, 0x0c0);
        }
      }
    }
    TreeClass::Render(rmsg);
  }
}

VOID LIClass::DecToRoman(ULONG number, STRPTR romanp)
{
  ULONG m = number/1000;

  /* Et antal M'er svarende til antal tusinde */
  while(m>0)
  {
    *romanp = 'M';
    romanp++;
    m--;
  }

  /* 1_34 - tredie ciffer, der skal blive til C, CC, CCC, CD, D, DC, DCC, DCCC eller CM */
  romanp = DigitToRoman(romanp, (number%1000)/100, 'M', 'D', 'C');
  /* 12_4 - anden ciffer, der skal blive til  X, XX, XXX, XL, L, LX, LXX, LXXX eller XC */
  romanp = DigitToRoman(romanp, (number%100)/10,   'C', 'L', 'X');
  /* 123_ - første ciffer, der skal blive til I, II, III, IV, V, VI, VII, VIII eller IX */
  romanp = DigitToRoman(romanp, (number%10),       'X', 'V', 'I');

  *romanp = '\0';
}

STRPTR LIClass::DigitToRoman(STRPTR romanp, UBYTE number, UBYTE a, UBYTE b, UBYTE c)
{
  switch(number)
  {
    case 0: case 1: case 2: case 3:
      while(number>0)
      {
        *romanp = c;
        romanp++;
        number--;
      }
    break;

    case 4:
      *romanp = c;
      romanp++;
      *romanp = b;
      romanp++;
    break;

    case 5: case 6: case 7: case 8:
      *romanp = b;
      romanp++;
      number-= 5;
      while(number>0)
      {
        *romanp = c;
        romanp++;
        number--;
      }
    break;

    case 9:
      *romanp = c;
      romanp++;
      *romanp = a;
      romanp++;
    break;
  }

  return(romanp);
}

