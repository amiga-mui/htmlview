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

#include "AreaClass.h"

#include "General.h"
#include "Map.h"
#include "ParseMessage.h"
#include "ScanArgs.h"

#include <proto/dos.h>

#include <ctype.h>
#include <math.h>
#include <stdlib.h>

BOOL AreaClass::UseMap (struct UseMapMessage &umsg)
{
  BOOL result = RC_NoMatch;
  switch(Shape)
  {
    case Area_Default:
      result = RC_Default;
    break;

    case Area_Rect:
    {
      if(umsg.X >= Rect.Left && umsg.X <= Rect.Right && umsg.Y >= Rect.Top && umsg.Y <= Rect.Bottom)
        result = RC_ExactMatch;
    }
    break;

    case Area_Circle:
    {
      LONG x = Circle.CenterX-umsg.X;
      LONG y = Circle.CenterY-umsg.Y;
      LONG radius = static_cast<LONG>(sqrt(x*x + y*y)+0.5);

      result = radius < Circle.Radius ? RC_ExactMatch : RC_NoMatch;
    }
    break;

    case Area_Poly:
    {
      LONG x = umsg.X, y = umsg.Y, inside = 0;

      struct CoordList *second, *first = Poly.Coords;
      while(first)
      {
        if(!(second = first->Next))
          second = Poly.Coords;

        LONG top, bottom, left, right;
        if(first->Y < second->Y)
        {
          top = first->Y; bottom = second->Y;
          left = first->X; right = second->X;
        }
        else
        {
          top = second->Y; bottom = first->Y;
          left = second->X; right = first->X;
        }

        /* y1 > y && y > y2 && (x1*(y-y2) + x2*(y1-y)) < x*(y1-y2) */
        if(top < y && bottom >= y)
        {
          if(max(first->X, second->X) <= x)
            inside++;
          else if(min(first->X, second->X) < x)
          {
            LONG divide_width = bottom-top;
            if(divide_width > 0)
            {
              LONG new_x = ((right - left) * (y - top)) / divide_width;
              if(new_x <= (x - left))
                inside++;
            }
            else
              inside++;
          }
        }
        first = first->Next;
      }
      result = inside & 1 ? RC_ExactMatch : RC_NoMatch;
    }
    break;
  }

  if(result)
  {
    if(Flags & FLG_Area_NoHRef)
    {
      umsg.URL = NULL;
    }
    else
    {
      umsg.URL = URL;
      umsg.Target = Target;
    }
  }
  return(result);
}

VOID AreaClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
  AttrClass::Parse(pmsg);
  pmsg.SkipSpaces();

  const char *ShapeKeywords[] = { "DEFAULT", "RECT", "POLY", "CIRCLE", NULL };
  ULONG shape = Area_Default;
  BOOL nohref = FALSE;
  STRPTR coords = NULL;
  struct ArgList args[] =
  {
    { "HREF",   &URL,     ARG_URL,      NULL },
    { "NOHREF", &nohref,  ARG_SWITCH,   NULL },
    { "TARGET", &Target,  ARG_URL,      NULL },
    { "COORDS", &coords,  ARG_STRING,   NULL },
    { "SHAPE",  &shape,   ARG_KEYWORD,  ShapeKeywords  },
    { NULL,     NULL,     0,            NULL }
  };
  ScanArgs(pmsg.Locked, args);

  if(nohref)
    Flags |= FLG_Area_NoHRef;

  Shape = shape;
  if(coords) switch(shape)
  {
    case Area_Rect:
    {
      LONG value, offset;
      STRPTR t_coords = coords;
      UWORD *storage = &Rect.Left;
      for(UWORD i = 0; i < 4; i++)
      {
        while(!isdigit(*t_coords) && *t_coords)
          t_coords++;

        offset = StrToLong(t_coords, &value);
        if(offset == -1 || !*t_coords)
        {
          i = 4;
        }
        else
        {
          *storage++ = value;
          t_coords += offset;
        }
      }
    }
    break;

    case Area_Poly:
    {
      LONG x, y, offset;
      STRPTR t_coords = coords;
      struct CoordList *prev, *current;
      prev = (struct CoordList *)&Poly.Coords;
      while(*t_coords)
      {
        offset = StrToLong(t_coords, &x);
        if(offset == -1)
          break;
        t_coords += offset;
        while(!isdigit(*t_coords) && *t_coords)
          t_coords++;

        offset = StrToLong(t_coords, &y);
        if(offset == -1)
          break;
        t_coords += offset;
        while(!isdigit(*t_coords) && *t_coords)
          t_coords++;

        current = new struct CoordList(x, y);
        prev->Next = current;
        prev = current;
      }
    }
    break;

    case Area_Circle:
    {
      ULONG centerx, centery, radius;
      if(sscanf(coords, "%ld,%ld,%ld", &centerx, &centery, &radius) == 3)
      {
        Circle.CenterX  = centerx;
        Circle.CenterY  = centery;
        Circle.Radius = radius;
      }
    }
    break;
  }
  delete coords;
}

