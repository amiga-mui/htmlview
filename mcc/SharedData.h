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

#ifndef SHAREDDATA_H
#define SHAREDDATA_H

#include "ImageManager.h"
#include "private.h"

struct SharedData
{
  SharedData (struct IClass *cl, Object *obj, struct HTMLviewData *data);

  VOID Setup (struct Screen *scr);
  BOOL InitConfig ();
  VOID Cleanup ();
  VOID FreeConfig ();
  VOID StopAnims (Object *obj);
  struct AnimInfo *AddAnim (Object *obj, struct HTMLviewData *data, struct PictureFrame *picture, struct ObjectList *receivers);

  /* Root object and it's instance data */
  Object *Obj;
  struct HTMLviewData *Data;

  /* Eventhandler note */
  struct MUI_EventHandlerNode Events;

  /* Imagedecode-pulse and gif-anim timer */
  struct MUI_PulseNode ImageTimer, AnimTimer;

  /* Listitem graphics */
  Object *ListItemMarkers;
  struct BitMap *LI_BMp;
  UBYTE *LI_Mask;
  ULONG LI_Width, LI_Height;

  /* Our screen */
  struct Screen *Scr;

  /* The fonts we've opened */
  struct TextFont *Fonts[Font_NumberOf];

  /* Key for page scroll + percent to scroll */
  struct InputXpression PageScrollKey;
  ULONG PageScrollMove;

  /* Pens that we've allocated */
  LONG Pens[Col_NumberOf];

  /* This is our decode-queue */
  class DecodeQueueManager DecodeQueue;

  /* Running anims */
  struct AnimInfo *Anims;

  /* This is just for the MUIA_HTMLview_ImagesInDecodeQueue */
  ULONG ImagesInDecodeQueue;

  /* Image cache */
  class ImageCache *ImageStorage;

  /* Image settings */
  ULONG GammaCorrection, DitherType, ImageCacheSize;

  /* Scroll deltas */
  ULONG HDeltaFactor, VDeltaFactor;

  /* Various flags */
  ULONG Flags;
};

#define FLG_CustomColours     (1 << 0)
#define FLG_SmoothPageScroll  (1 << 1)
#define FLG_NewConfig         (1 << 2)

#endif
