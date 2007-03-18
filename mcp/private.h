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

#ifndef HTMLVIEW_MCP_PRIV_H
#define HTMLVIEW_MCP_PRIV_H

#include "HTMLview_mcp.h"

#include <mcc_common.h>

#define PREFSIMAGEOBJECT \
  BitmapObject,\
    MUIA_Bitmap_Bitmap,       (UBYTE *)&image_bitmap,\
    MUIA_Bitmap_Height,       IMAGE_HEIGHT,\
    MUIA_Bitmap_Precision,    0,\
    MUIA_Bitmap_SourceColors, (ULONG *)image_palette,\
    MUIA_Bitmap_Transparent,  0,\
    MUIA_Bitmap_Width,        IMAGE_WIDTH,\
    MUIA_FixHeight,           IMAGE_HEIGHT,\
    MUIA_FixWidth,            IMAGE_WIDTH,\
  End

#define MCPMAXRAWBUF 64

#define IEQUALIFIER_SHIFT   0x0200
#define IEQUALIFIER_ALT     0x0400
#define IEQUALIFIER_COMMAND 0x0800

enum
{
	SmallFont = 0, NormalFont,
	FixedFont, LargeFont,
	H1, H2, H3, H4, H5, H6,

	IgnoreDocCols,
	Col_Background, Col_Text,
	Col_Link, Col_VLink, Col_ALink,

	ListItemFile,

	DitherType,
	ImageCacheSize,
	GammaCorrection,

	PageScrollSmooth, PageScrollKey, PageScrollMove,

	NumberOfObjects
};

struct InstData_MCP
{
	Object *Objects[NumberOfObjects];
};

extern struct MUI_CustomClass *CacheSliderClass;
extern struct MUI_CustomClass *GammaSliderClass;

Object *CreatePrefsGroup(struct InstData_MCP *data);
BOOL CreateSubClasses(void);
void DeleteSubClasses(void);

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

#endif /* HTMLVIEW_MCP_PRIV_H */
