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

#ifndef CLASSES_H
#define CLASSES_H

#include <intuition/classusr.h>
#include <proto/graphics.h>
#include <proto/muimaster.h>

#include "TagInfo.h"
#include "AppendGadget.h"
#include "GetImages.h"
#include "Render.h"
#include "HitTest.h"
#include "Mark.h"

#include "SDI_compiler.h"

/* Protos */

class SuperClass *CreateObject (struct TagInfo *tag);

/* Defines */

#define FLG_AllElementsPresent    (1 << 0)
#define FLG_Layouted            (1 << 1)
#define FLG_Attribute         (1 << 2)
#define FLG_Tree              (1 << 3)
#define FLG_Virgin            (1 << 4)
#define FLG_Newline           (1 << 5)
#define FLG_WaitingForSize        (1 << 6)
#define FLG_AllocatedColours      (1 << 7)
#define FLG_KnowsMinMax         (1 << 8)
#define FLG_Gadget            (1 << 9)
#define FLG_ArgumentsRead       (1 << 10)


#define FLG_A_Visited         (1 << 11)
#define FLG_A_QueryDone         (1 << 12)

#define FLG_Area_NoHRef         (1 << 11)

#define FLG_Frame_NoResize        (1 << 11)
#define FLG_Frame_PageLoaded      (1 << 12)
#define FLG_Frame_ObjAdded        (1 << 13)
#define FLG_Frame_BorderLeft      (1 << 14)
#define FLG_Frame_BorderTop     (1 << 15)

#define FLG_HR_NoShade          (1 << 11)

#define FLG_Img_IsMap         (1 << 11)
#define FLG_Img_DrawBackground    (1 << 12)
#define FLG_Img_CreateAlpha     (1 << 13)

#define FLG_Input_Checked       (1 << 11)
#define FLG_Input_Disabled        (1 << 12)
#define FLG_Input_ReadOnly        (1 << 13)
#define FLG_Input_ObjAdded        (1 << 14)

#define FLG_IsIndex_ObjAdded      (1 << 11)

#define FLG_Option_Disabled     (1 << 11)
#define FLG_Option_Selected     (1 << 12)

#define FLG_Select_Disabled     (1 << 11)
#define FLG_Select_Multiple     (1 << 12)
#define FLG_Select_ObjAdded     (1 << 13)

#define FLG_TD_NoWrap         (1 << 11)

#define FLG_Text_Pre            (1 << 11)

#define FLG_TextArea_Disabled     (1 << 11)
#define FLG_TextArea_ReadOnly     (1 << 12)
#define FLG_TextArea_ObjAdded     (1 << 13)

/* Data */

enum
{
  Font_None = -1,
  Font_Normal,
  Font_H1,
  Font_H2,
  Font_H3,
  Font_H4,
  Font_H5,
  Font_H6,
  Font_Fixed,
  Font_Big,
  Font_Small,
  Font_NumberOf
};

enum
{
  Align_None = 0,
  Align_Left,
  Align_Center,
  Align_Right,
  Align_Top,
  Align_Middle,
  Align_Bottom,
  Align_Query
};

enum
{
  TSF_Underline = 1,
  TSF_Bold = 2,
  TSF_Italic = 4,
  TSF_Link = 8,
  TSF_VLink = 16,
  TSF_ALink = 32
/*  TSF_Strike = 64,
  TSF_Sub = 128,
  TSF_Super = 256,
*/
};

#define TSF_StyleMask (TSF_Underline | TSF_Bold | TSF_Italic)

enum
{
  AboveLayer,
  BelowLayer,
  InsideLayer
};

struct ArgSize
{
  ArgSize () { ; }
  ArgSize (ULONG size, ULONG type) { Size = size; Type = type; }

  ULONG Size;
  ULONG Type;
};

enum
{
  Size_Pixels,
  Size_Percent,
  Size_Relative
};

struct ChildsList
{
  ChildsList (class SuperClass *obj = NULL)
  {
    Next = NULL;
    Obj = obj;
  }

  struct ChildsList *Next;
  class SuperClass *Obj;
};

struct TextLineInfo
{
  TextLineInfo (LONG left, LONG length)
  {
    Left = left;
    Length = length;
  }

  struct TextLineInfo *Next;
  LONG Left, Baseline, Length;
};

struct FindMessage
{
  STRPTR SearchString;
  STRPTR SearchString_EOL;
  ULONG StrLength;
  int (*StrCmp) (const char *, const char *, size_t);

  ULONG Flags;

  ULONG TopPos;
  class TextClass *LastMatch;
  struct MarkMessage *MarkMsg;
  BYTE SearchMap[256];
};

#define FND_CaseSensitive (1 << 0)
#define FND_Pattern     (1 << 1)
#define FND_Backwards   (1 << 2)
#define FND_FromTop     (1 << 3)
#define FND_Next        (1 << 4)




enum
{
  Area_Default = 0,
  Area_Rect,
  Area_Poly,
  Area_Circle
};

struct CoordList
{
  CoordList (UWORD x, UWORD y)  { X = x, Y = y; }
  ~CoordList ()           { delete Next; }

  struct CoordList *Next;
  UWORD X, Y;
};












enum
{
  Form_Get = 0,
  Form_Post
};







enum
{
  Input_Text = 0,
  Input_Password,
  Input_Checkbox,
  Input_Radio,
  Input_Submit,
  Input_Reset,
  Input_File,
  Input_Hidden,
  Input_Image,
  Input_Button
};

enum
{
  OL_Arabic = 0,
  OL_LowerAlpha,
  OL_UpperAlpha,
  OL_LowerRoman,
  OL_UpperRoman
};

extern const char *const AlignKeywords[];
extern const char *const VAlignKeywords[];
extern const char *const ImgAlignKeywords[];

#endif
