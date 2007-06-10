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

#ifndef SCANARGS_H
#define SCANARGS_H

VOID ScanArgs (STRPTR tag, struct ArgList *args);

struct ArgList
{
  const char *Name;
  APTR Storage;
  ULONG Type;
  const char *const *KeywordList;
};

enum
{
  ARG_STRING,
  ARG_NUMBER,
  ARG_COLOUR,
  ARG_URL,
  ARG_VALUE,
  ARG_MULTIVALUE,
  ARG_KEYWORD,
  ARG_SWITCH,
  ARG_BOOLEAN
};

#endif
