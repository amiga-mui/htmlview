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

#ifndef PRIVATE_H
#define PRIVATE_H

#ifndef _PROTO_INTUITION_H
#include <proto/intuition.h>
#endif

#ifndef LIBRARIES_MUI_H
#include <libraries/mui.h>
#endif

#include <libraries/commodities.h>

/* Other includes */

#include "SDI_compiler.h"

#include "HTMLview_mcc.h"
#include "General.h"
#include "Layout.h"
#include "Render.h"
#include "Classes.h"

#include "Debug.h"

#include "mcc_common.h"

struct MUI_PulseNode
{
  struct MUI_InputHandlerNode ihn;

  VOID Start (Object *obj);
  BOOL Stop (Object *obj);

  BOOL Running;
};

struct MUIP_HTMLview_Post
{
  ULONG MethodID;
  STRPTR URL, Target, Data, EncodingType;
};

struct MUIP_HTMLview_LoadImages
{
  ULONG MethodID;
  struct ImageList *Images;
};

struct MUIP_HTMLview_LookupFrame
{
  ULONG MethodID;
  STRPTR Name;
};

struct MUIP_HTMLview_HitTest
{
  ULONG MethodID;
  struct HitTestMessage *HMsg;
};

struct MUIP_HTMLview_AddSingleAnim
{
  ULONG MethodID;
  struct PictureFrame *picture;
  class SuperClass *receiver;
};

struct MUIP_HTMLview_ServerRequest
{
  ULONG MethodID;
  STRPTR Argument;
};

/* InstanceData */

struct HTMLviewData
{
  ULONG Flags;
  struct SharedData *Share;

  struct LayoutMessage LayoutMsg;
  struct RenderMessage RenderMsg;

  struct Hook *LoadHook, *ImageLoadHook;

  ULONG Left, Top, Width, Height, XOffset, YOffset, VirtWidth, VirtHeight;
  ULONG TopChange;
  struct MUI_PulseNode Period, RefreshTimer;

  struct MsgPort *MessagePort;
  struct MUI_PulseNode ihnode;

  struct Process *ParseThread;
  struct ParseMessage *PMsg;

  /* Images waiting to get decoded... */
  struct ImageList *Images, *ImagesLeft;
  ULONG RunningDecoderThreads;

  /* The object to redraw, during MUIM_Draw */
  class SuperClass *RedrawObj;

  class HostClass *HostObject;
  ULONG ParseCount;
  ULONG PageID;
  STRPTR Title;

  STRPTR URLBase; /* Domain */
  STRPTR Page;    /* Page name */
  STRPTR Local;   /* Relative to page */
  STRPTR URL;     /* Entire URL */

  STRPTR FrameName; /* The name of this frame */
  STRPTR Target;    /* Temporary target name */

  STRPTR PostData;    /* These fields are set by MUIM_HTMLview_Post, */
  STRPTR EncodingType;  /* and read by MUIM_HTMLview_StartParser */

  STRPTR LastURL, ActiveURL;
  class SuperClass *ActiveURLObj;
  ULONG Qualifier;

  LONG HScrollDiff, VScrollDiff;

  char ParseThreadName[32];

  ULONG Parsed;

  /* Resulting structure for GetContextInfo */
  struct MUIR_HTMLview_GetContextInfo ContextInfo;
};

#define FLG_RootObj           (1<<0)
#define FLG_HostObjNotUsed    (1<<1)
#define FLG_ReAllocColours    (1<<2)
#define FLG_NotResized        (1<<3)
#define FLG_Shown             (1<<4)
#define FLG_ActiveLink        (1<<5)
#define FLG_Reload            (1<<6)
#define FLG_NoBackfill        (1<<7)
#define FLG_RemoveChildren    (1<<9)
#define FLG_DiscreteInput     (1<<10)
#define FLG_MarkingEnabled    (1<<11)
#define FLG_RedrawMarked      (1<<12)

#define STACKSIZEPPC 32768
#define STACKSIZE68K 16384

extern "C" DISPATCHERPROTO(_Dispatcher);

#endif
