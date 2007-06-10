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

#ifndef FRAMES_H
#define FRAMES_H

enum
{
  NoFrameBorder = 0,
  FrameBorder,
  OuterBox
};

struct DivideBordersMessage
{
  DivideBordersMessage ()
  {
    LeftBorder = OuterBox;
    TopBorder = OuterBox;
  }

  /* Result */
  BOOL LeftBorder, TopBorder;
};

#endif
