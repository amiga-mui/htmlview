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

//#include <clib/alib_protos.h>
//#include <datatypes/datatypes.h>
#include <datatypes/pictureclass.h>
//#include <datatypes/pictureclassext.h>
#include <proto/commodities.h>
#include <proto/datatypes.h>
#include <proto/diskfont.h>
#include <proto/intuition.h>
#include <libraries/mui.h>
#include <graphics/text.h>

#include "General.h"
//#include "Preference/ConfigItems.h"
#include "ImageManager.h" /* struct ImageCache */
#include "SharedData.h"
//#include "private.h"

#include "HTMLview_mcp.h"

#include <string.h>
#include <stdio.h>
#include <new>

#define GetConfigItem(i, v) (APTR)GetConfigItemA(Obj, i, (ULONG)v, FALSE)
#define GetConfigVal(i, v) GetConfigItemA(Obj, i, (ULONG)v, TRUE)

ULONG GetConfigItemA (Object *obj, ULONG item, ULONG def_value, BOOL de_ref)
{
  ULONG value;
  return DoMethod(obj, MUIM_GetConfigItem, (ULONG)item, (ULONG)&value) ? (de_ref ? *(ULONG *)value : value) : def_value;
}

static BOOL GetFont (Object *Obj, ULONG item, CONST_STRPTR def_value, struct TextFont **storage)
{
  struct TextFont *oldfont = *storage;
  ULONG size;
  char base[200];
  STRPTR font = (STRPTR)GetConfigItem(item, def_value);
  if(2 == sscanf(font, "%[^/]/%ld", base, &size))
  {
    strcat(base, ".font");
    struct TextAttr ta = { base, size, 0, 0 };
    *storage = OpenDiskFont(&ta);
  }
  else
  {
    *storage = NULL;
  }

  if(!*storage)
  {
    struct TextAttr ta;
    AskFont(&_screen(Obj)->RastPort, &ta);
    *storage = OpenDiskFont(&ta);
  }
  return(oldfont != *storage);
}

static VOID GetColour (Object *Obj, ULONG item, CONST_STRPTR def_value, LONG *storage)
{
  struct MUI_PenSpec *spec = (struct MUI_PenSpec *)GetConfigItem(item, def_value);
  *storage = MUI_ObtainPen(muiRenderInfo(Obj), spec, 0L);
}

Object *DecodeListItemGfx (struct Screen *scr, STRPTR file)
{
  Object *dt;
  if((dt = NewDTObject(file,
    DTA_GroupID,          GID_PICTURE,
    PDTA_DestMode,        PMODE_V43,
    PDTA_Remap,           TRUE,
    PDTA_FreeSourceBitMap,TRUE,
    PDTA_Screen,          (ULONG)scr,
    PDTA_UseFriendBitMap, TRUE,
    TAG_DONE)))
  {
    DoMethod(dt, DTM_PROCLAYOUT, NULL, FALSE);
  }
  return(dt);
}

BOOL SharedData::InitConfig ()
{
  DitherType = GetConfigVal(MUICFG_HTMLview_DitherType, 0);
  ImageCacheSize = 1024 * GetConfigVal(MUICFG_HTMLview_ImageCacheSize, 512);
  GammaCorrection = GetConfigVal(MUICFG_HTMLview_GammaCorrection, 2200);
  ImageStorage = new (std::nothrow) class ImageCache(ImageCacheSize);
  if (!ImageStorage) return FALSE;

  if(GetConfigVal(MUICFG_HTMLview_IgnoreDocCols, FALSE))
      Flags |= FLG_CustomColours;
  else  Flags &= ~FLG_CustomColours;

  if(GetConfigVal(MUICFG_HTMLview_PageScrollSmooth, TRUE))
      Flags |= FLG_SmoothPageScroll;
  else  Flags &= ~FLG_SmoothPageScroll;

  STRPTR scrollkey = (STRPTR)GetConfigItem(MUICFG_HTMLview_PageScrollKey, "-repeat space");
  PageScrollMove = GetConfigVal(MUICFG_HTMLview_PageScrollMove, 70);
  ParseIX(scrollkey, &PageScrollKey);

  BOOL newconfig;
  newconfig  = GetFont(Obj, MUICFG_HTMLview_NormalFont, "XHelvetica/13",  &Fonts[Font_Normal]);
  newconfig |= GetFont(Obj, MUICFG_HTMLview_H1,     "Helvetica/24", &Fonts[Font_H1]);
  newconfig |= GetFont(Obj, MUICFG_HTMLview_H2,     "Helvetica/18", &Fonts[Font_H2]);
  newconfig |= GetFont(Obj, MUICFG_HTMLview_H3,     "Helvetica/15", &Fonts[Font_H3]);
  newconfig |= GetFont(Obj, MUICFG_HTMLview_H4,     "XHelvetica/13",  &Fonts[Font_H4]);
  newconfig |= GetFont(Obj, MUICFG_HTMLview_H5,     "XHelvetica/11",  &Fonts[Font_H5]);
  newconfig |= GetFont(Obj, MUICFG_HTMLview_H6,     "XHelvetica/9", &Fonts[Font_H6]);
  newconfig |= GetFont(Obj, MUICFG_HTMLview_FixedFont,  "Xen/11",     &Fonts[Font_Fixed]);
  newconfig |= GetFont(Obj, MUICFG_HTMLview_LargeFont,  "Helvetica/24", &Fonts[Font_Big]);
  newconfig |= GetFont(Obj, MUICFG_HTMLview_SmallFont,  "XHelvetica/9", &Fonts[Font_Small]);

  GetColour(Obj, MUICFG_HTMLview_Col_Background,  "m1",                   &Pens[Col_Background]);
  GetColour(Obj, MUICFG_HTMLview_Col_Text,      "m5",                   &Pens[Col_Text]);
  GetColour(Obj, MUICFG_HTMLview_Col_Link,      "r00000000,00000000,99999999",  &Pens[Col_Link]);
  GetColour(Obj, MUICFG_HTMLview_Col_VLink,     "rAAAAAAAA,00000000,00000000",  &Pens[Col_VLink]);
  GetColour(Obj, MUICFG_HTMLview_Col_ALink,     "rDCDCDCDC,DCDCDCDC,00000000",  &Pens[Col_ALink]);

  ULONG rgb[3];
  struct ColorMap *cmap = Scr->ViewPort.ColorMap;
  GetRGB32(cmap, MUIPEN(Pens[Col_Background]), 1, rgb);
  ULONG colour = ((rgb[0] >> 8) & 0xff0000) | ((rgb[1] >> 16) & 0xff00) | ((rgb[2] >> 24) & 0xff);
  ObtainShineShadowPens(cmap, colour, Pens[Col_Halfshine], Pens[Col_Halfshadow]);

  Pens[Col_Shine] = _pens(Obj)[MPEN_SHINE];
  Pens[Col_Shadow] = _pens(Obj)[MPEN_SHADOW];

  if((ListItemMarkers = DecodeListItemGfx(Scr, (STRPTR)GetConfigItem(MUICFG_HTMLview_ListItemFile, "ProgDir:Images/ListItems"))))
  {
    LI_Mask = NULL;
    struct BitMapHeader *header;
    GetDTAttrs(ListItemMarkers, PDTA_MaskPlane, (ULONG)&LI_Mask, PDTA_BitMapHeader, (ULONG)&header, PDTA_DestBitMap, (ULONG)&LI_BMp, TAG_DONE);

    if(LI_Width != (header->bmh_Width / 3) || LI_Height != header->bmh_Height)
      newconfig = TRUE;

    LI_Width = header->bmh_Width / 3;
    LI_Height = header->bmh_Height;
  }

  struct TextFont *font = Fonts[Font_Normal];
  HDeltaFactor = MyTextLength(font, (STRPTR)"n", 1);
  VDeltaFactor = font->tf_YSize + 1;

  if(newconfig)
    Flags |= FLG_NewConfig;

  return(FALSE);
}

VOID SharedData::FreeConfig ()
{
  for(int index = Font_Normal; index < Font_NumberOf; index++)
  {
    if(Fonts[index])
      CloseFont(Fonts[index]);
  }

  for(int index = Col_Background; index <= Col_ALink; index++)
    MUI_ReleasePen(muiRenderInfo(Obj), Pens[index]);

  struct ColorMap *cmap = Scr->ViewPort.ColorMap;
  ReleasePen(cmap, Pens[Col_Halfshine]);
  ReleasePen(cmap, Pens[Col_Halfshadow]);

  if(ListItemMarkers)
    DisposeDTObject(ListItemMarkers);
  LI_BMp = NULL;

  delete ImageStorage;
  ImageStorage = NULL;
}
