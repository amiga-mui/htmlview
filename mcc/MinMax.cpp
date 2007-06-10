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

#include <string.h>
#include <exec/types.h>
#include <clib/alib_protos.h>
#include <datatypes/pictureclass.h>
#include <libraries/mui.h>

#include "General.h"
#include "Classes.h"
#include "MinMax.h"
#include "Layout.h"
#include "ImageManager.h" /* class ImageCache */
#include "IM_Render.h"    /* struct PictureFrame */
#include "SharedData.h"

#include "private.h"

MinMaxMessage::MinMaxMessage (struct TextFont **fonts, struct LayoutMessage *lmsg, struct CellWidth *widths)
{
  Fonts = fonts;
  LMsg = lmsg;
  Widths = widths;
}

VOID MinMaxMessage::Reset ()
{
  Min = Max = X = Indent = ImageLeftIndent = ImageRightIndent = 0;
  Font = Fonts[Font_Normal];
}

VOID MinMaxMessage::Newline ()
{
  Max = max((LONG)X, Max);
  X = Indent;
}
