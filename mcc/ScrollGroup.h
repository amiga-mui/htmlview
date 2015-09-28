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

#ifndef SCROLLGROUP_H
#define SCROLLGROUP_H

#include "SDI_compiler.h"
#include "SDI_hook.h"

#ifdef __cplusplus
extern "C" {
#endif

extern DISPATCHERPROTO(ScrollGroupDispatcher);
extern struct MUI_CustomClass *ScrollGroupClass;

#ifdef __cplusplus
}
#endif

#define MUIA_ScrollGroup_HTMLview         HTMLview_ID(100) /* Private */
#define MUIA_ScrollGroup_Scrolling        HTMLview_ID(101) /* Private */
#define MUIA_ScrollGroup_Smooth           HTMLview_ID(102) /* Private */
#define MUIA_ScrollGroup_Frames           HTMLview_ID(103) /* Private */
#define MUIM_ScrollGroup_NewWidth         HTMLview_ID(104) /* Private */
#define MUIM_ScrollGroup_NewHeight        HTMLview_ID(105) /* Private */
#define ScrollGroup_NumberOf              HTMLview_ID(106)

struct ScrollGroupData
{
  Object *HTMLview;
  Object *Knob;
  Object *RightScroll;
  Object *BottomScroll;
  ULONG   Flags;
  struct MUI_EventHandlerNode Events;
  LONG    KnobPressX;
  LONG    KnobPressY;
  LONG    VScrollValue;
  LONG    HScrollValue;
};

#define FLG_RightVisible  (1 << 0)
#define FLG_BottomVisible (1 << 1)
#define FLG_HorizAuto     (1 << 2)
#define FLG_SmoothScroll  (1 << 3)
#define FLG_KnobVisible   (1 << 4)

#endif
