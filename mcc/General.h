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

#ifndef GENERAL_H
#define GENERAL_H

#include <intuition/classusr.h>

struct HTMLviewData;

extern STRPTR ImageDecoderName;

LONG MyTextLength(struct TextFont *font, STRPTR text, LONG length);
LONG MyTextFit(struct TextFont *font, STRPTR text, LONG length, LONG width, LONG direction);

/* HandleMUIEvent.c */
STRPTR FindTarget (Object *obj, STRPTR target, struct HTMLviewData *data);

BOOL InitConfig (struct IClass *cl, Object *obj, struct HTMLviewData *data);
VOID FreeConfig (struct IClass *cl, Object *obj, struct HTMLviewData *data);

VOID ObtainShineShadowPens (struct ColorMap *cmap, ULONG rgb, LONG &shine, LONG &shadow);
VOID BltMaskRPort (struct BitMap *source, WORD srcLeft,WORD srcTop, struct RastPort *destination, WORD dstLeft,WORD dstTop,WORD dstWidth,WORD dstHeight, UBYTE *maskPlane);

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)

/// xget()
//  Gets an attribute value from a MUI object
ULONG xget(Object *obj, const ULONG attr);
#if defined(__GNUC__) || ((__STDC__ == 1L) && (__STDC_VERSION__ >= 199901L))
  // please note that we do not evaluate the return value of GetAttr()
  // as some attributes (e.g. MUIA_Selected) always return FALSE, even
  // when they are supported by the object. But setting b=0 right before
  // the GetAttr() should catch the case when attr doesn't exist at all
  #define xget(OBJ, ATTR) ({ULONG b=0; GetAttr(ATTR, OBJ, &b); b;})
#endif
///

#define MUIA_CustomBackfill                 0x80420a63
#define MUIM_Backfill                       0x80428d73
struct  MUIP_Backfill { ULONG MethodID; LONG left; LONG top; LONG right; LONG bottom; LONG xoffset; LONG yoffset; LONG brightness; };

#define MUIM_Group_ExitChange2              0x8042e541 /* private */ /* V12 */
struct  MUIP_Group_ExitChange2              { ULONG MethodID; ULONG flags; }; /* private */

#define MUIA_Prop_DeltaFactor   0x80427C5E
#define MUIA_Group_Forward    0x80421422 /* V11 .s. BOOL */

#define MADF_DRAWOUTER         (1<<11) /* private */

#define MAX_HEIGHT 0x0fffffff

#define Precision(col) ((col) << 24 | (col) << 16 | (col) << 8 | (col))

#endif
