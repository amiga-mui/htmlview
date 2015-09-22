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

#ifndef LAYOUT_H
#define LAYOUT_H

/* For frame layout */
enum
{
  Const_NoFrame = 0,
  Const_Frame,
  Const_ForceNoFrame
};

struct FloadingImage
{
  FloadingImage (LONG top, LONG &left, LONG width, LONG height, class SuperClass *obj, class SuperClass *container)
  {
    Top = top;
    Left = &left;
    Width = width;
    Height = height;
    Obj = obj;
    Container = container;
  }

  struct FloadingImage *Next;
  class SuperClass *Obj, *Container;
  LONG Top, *Left, Width, Height;
};

struct ObjectNotify
{
  ObjectNotify (LONG &left, LONG &baseline, class SuperClass *obj)
  {
    Left = &left;
    Baseline = &baseline;
    Obj = obj;
  }

  ~ObjectNotify ()
  {
    delete Next;
  }

  struct ObjectNotify *Next;
  LONG *Left, *Baseline;
  class SuperClass *Obj;
};

struct GadgetList
{
  GadgetList (class SuperClass *obj)
  {
    GadgetObject = obj;
  }
  ~GadgetList ()
  {
    delete Next;
  }

  struct GadgetList *Next;
  class SuperClass *GadgetObject;
};

enum
{
  Flush_Left = 1,
  Flush_Right = 2,
  Flush_All = 3
};

struct LayoutMessage
{
  VOID Reset (ULONG width, ULONG height);
  VOID Newline ();
  BOOL IsAtNewline ();
  LONG ScrWidth ();
  VOID EnsureNewline ();
  VOID AddYSpace (ULONG space);
  LONG AddImage (struct FloadingImage *img, BOOL place);
  VOID FlushImages (ULONG place);
  VOID AddNotify (struct ObjectNotify *obj);
  VOID UpdateBaseline (LONG height, LONG baseline);
  VOID AddToGadgetList (class SuperClass *gadget);
  VOID FlushGadgetList ();
  VOID SetLineHeight (LONG height);
  VOID CheckFloatingObjects ();

  struct SharedData *Share;

  /* Linked lists of gadgets, MUI wants me to constantly re-position them... */
  struct GadgetList *FirstGadget;
  struct GadgetList *LastGadget;

  /* The host object, used for AddPart() and VLink() */
  Object *HTMLview;

  /* Objects to know about the ending coordinates */
  struct ObjectNotify *Notify;
  struct ObjectNotify *NotifyLast;

  /* Visible frame */
  LONG MinX, MaxX, MinY, MaxY;
  LONG ScrHeight;

  /* Margin, adjustable by frame-elements */
  LONG MarginWidth, MarginHeight;

  /* Indentation */
  LONG Indent;
  LONG ImageLeftIndent, ImageRightIndent;
  LONG LIIndent; /* How much should <LI> add? */
  UWORD OL_Type;

  /* Current styles */
  UBYTE Align;
  UBYTE padding[1];

  /* Current position */
  LONG X, Y;

  /* Flooding images */
  struct FloadingImage *FLeft;
  struct FloadingImage *FRight;
  class SuperClass *Parent;

  /* Reflects the current (text) line */
  LONG Baseline, Bottom, MinLineHeight;

  /* Total size of the page */
  LONG Width, Height;

  LONG TopChange;

  /* Current font */
  struct TextFont *Font;

  /* For table layout */
  struct CellWidth *Widths;
  ULONG *Heights, *RowOpenCounts;;
  LONG Spacing, Padding, Columns;
  BOOL TableBorder, Pass;

  /* For frame layout */
  BOOL LeftBorder, TopBorder;
};

#endif
