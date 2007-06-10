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

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <datatypes/datatypes.h>
#include <datatypes/pictureclass.h>
//#include <datatypes/pictureclassext.h>
#include <clib/alib_protos.h>
#include <proto/datatypes.h>
#include <proto/exec.h>
#include <proto/graphics.h>

#include "General.h"
#include "Layout.h"
#include "TagInfo.h"
#include "MinMax.h"
#include "Classes.h"
#include "ImageManager.h" /* class ImageCache */
#include "IM_Render.h"    /* struct PictureFrame */
#include "SharedData.h"

#include "classes/ImgClass.h"
#include "classes/SuperClass.h"

#include "private.h"

/*
extern "C" ULONG StackReg ();
ULONG LayoutStack = 0;
VOID StackCheck ()
{
  LayoutStack = max(LayoutStack, (ULONG)FindTask(NULL)->tc_SPUpper-StackReg());
}*/

VOID LayoutMessage::Reset (ULONG width, ULONG height)
{
  MinX = X = 0;
  MinY = Y = 0;
  MaxX = width  - 1;
  MaxY = height - 1;

  Width = Height = 0;
  MinLineHeight = Baseline = Bottom = 0;
  Indent = ImageLeftIndent = ImageRightIndent = 0;

  Font = Share->Fonts[Font_Normal];

  delete Notify;
  Notify = NULL;
  NotifyLast = (struct ObjectNotify *)&Notify;

  struct FloadingImage *del, *img = FRight;
  while(img)
  {
    del = img;
    img = img->Next;
    delete del;
  }
  img = FLeft;
  while(img)
  {
    del = img;
    img = img->Next;
    delete del;
  }
  FLeft = FRight = NULL;

  LeftBorder = Const_ForceNoFrame;
  TopBorder = Const_ForceNoFrame;
}

VOID LayoutMessage::EnsureNewline ()
{
  if(X > MinX+ImageLeftIndent+Indent)
    Newline();
}

BOOL LayoutMessage::IsAtNewline ()
{
  return(X == MinX+ImageLeftIndent+Indent);
}

LONG LayoutMessage::ScrWidth ()
{
  return(MaxX - X + 1);
}

VOID LayoutMessage::Newline ()
{
  class SuperClass *obj;
  LONG baseline = Y + Baseline;
  struct ObjectNotify *notify = Notify;
  while(notify)
  {
    LONG delta;
    if((delta = MaxX+1-X) > 0)
    {
      switch(Align)
      {
        case Align_Center:
          *notify->Left += delta/2;
        break;

        case Align_Right:
          *notify->Left += delta;
        break;
      }
    }

    *notify->Baseline = baseline;
    if((obj = notify->Obj))
    {
      if(obj->id() != tag_IMG || ((class ImgClass *)obj)->Alignment != Align_Top)
      {
        obj->setTop(obj->top() + Baseline);
        obj->setBottom(obj->bottom() + Baseline);
      }
      obj->setFlags(obj->flags() & ~FLG_WaitingForSize);
      obj->setFlags(obj->flags() | FLG_Layouted);
      TopChange = min(TopChange, obj->top());

/*      if(obj->flags() & FLG_Gadget)
        obj->AdjustPosition(0, 0);
*/    }
    notify = notify->Next;
  }

  ULONG lineheight = max(Baseline + Bottom, MinLineHeight);
  Y += lineheight;
  Width = max(Width, X+MarginWidth+ImageRightIndent);
  Height = Y;

  if(FRight && X > *FRight->Left)
  {
    LONG delta = X - *FRight->Left;
    struct FloadingImage *img = FRight;
    while(img)
    {
      *img->Left += delta;
      img = img->Next;
    }
  }

  CheckFloatingObjects();

  MinLineHeight = Baseline = Bottom = 0;

  delete Notify;
  Notify = NULL;
  NotifyLast = (struct ObjectNotify *)&Notify;
}

VOID LayoutMessage::CheckFloatingObjects ()
{
  while(FLeft && (Y >= (FLeft->Top + FLeft->Height)))
  {
    struct FloadingImage *img = FLeft;
    FLeft = FLeft->Next;
    ImageLeftIndent -= img->Width;
    if(ImageLeftIndent == 5)
      ImageLeftIndent = 0;
    img->Obj->setFlags(img->Obj->flags() | FLG_Layouted);
    /* This line is new, does it work? */
    TopChange = min(TopChange, img->Obj->top());
    delete img;
  }

  while(FRight && (Y >= (FRight->Top + FRight->Height)))
  {
    struct FloadingImage *img = FRight;
    FRight = FRight->Next;
    ImageRightIndent -= img->Width;
    MaxX += img->Width;
    img->Obj->setFlags(img->Obj->flags() | FLG_Layouted);
    /* This line is new, does it work? */
    TopChange = min(TopChange, img->Obj->top());
    delete img;
  }

  X = MinX+ImageLeftIndent+Indent;
}

VOID LayoutMessage::AddYSpace (ULONG space)
{
  EnsureNewline();
  Y += space;
}

LONG LayoutMessage::AddImage (struct FloadingImage *img, BOOL place)
{
  LONG pos;
  if(place)
  {
    img->Next = FRight;
    FRight = img;

    ImageRightIndent += img->Width;
//    pos = MaxX - ImageRightIndent;
    MaxX -= img->Width;
    pos = MaxX+1;
  }
  else
  {
    img->Next = FLeft;
    FLeft = img;

    if(!ImageLeftIndent)
      ImageLeftIndent += 5;
    pos = MinX+ImageLeftIndent-5;
    ImageLeftIndent += img->Width;
    X = MinX+ImageLeftIndent+Indent;
  }
  return(pos);
}

VOID LayoutMessage::FlushImages (ULONG place)
{
  if(place & Flush_Left)
  {
    while(FLeft)
    {
      LONG height = (FLeft->Top + FLeft->Height) - Y;
      UpdateBaseline(height, height-1);
      Newline();
    }
  }

  if(place & Flush_Right)
  {
    while(FRight)
    {
      LONG height = (FRight->Top + FRight->Height) - Y;
      UpdateBaseline(height, height-1);
      Newline();
    }
  }
}

VOID LayoutMessage::AddNotify (struct ObjectNotify *obj)
{
  NotifyLast->Next = obj;
  NotifyLast = obj;
}

VOID LayoutMessage::UpdateBaseline (LONG height, LONG baseline)
{
  Baseline = max(Baseline, baseline);
  Bottom = max(Bottom, height-baseline);
}

VOID LayoutMessage::SetLineHeight (LONG height)
{
  MinLineHeight = max(MinLineHeight, height);
}

VOID LayoutMessage::AddToGadgetList (class SuperClass *gadget)
{
  LastGadget->Next = new struct GadgetList(gadget);
  LastGadget = LastGadget->Next;
}

VOID LayoutMessage::FlushGadgetList ()
{
  delete FirstGadget;

  FirstGadget = NULL;
  LastGadget = (struct GadgetList *)&FirstGadget;
}
