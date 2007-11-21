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

#include <ctype.h>
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <libraries/mui.h>
#include <mui/TextEditor_mcc.h>

#include "General.h"
#include "Classes.h"
#include "Forms.h"
#include "private.h"
#include "SharedData.h"
#include <stdio.h>

BOOL IsCharSafe (UBYTE byte)
{
  BOOL res = isalnum(byte) || byte == ' ';
/*  if(byte < 39 || byte == 127) // [0-31] | [ !"#$%&] | DEL
    res = byte == '!' || byte == ' ';
  else switch(byte)
  {
    case '/':
    case '?':
    case ':':
    case '@':
    case '=':
    case '+':
    case '<':
    case '>':
      res = FALSE;
    break;

    default:
      res = TRUE;
    break;
  }*/
  return(res);
}

STRPTR EncodeString (STRPTR src)
{
  ULONG len = 1;

  STRPTR t_str = src;
  while(*t_str)
    len += IsCharSafe(*t_str++) ? 1 : 3;

  STRPTR dst, encoded = dst = new char[len];
  while(*src)
  {
    UBYTE byte = *src++;
    if(IsCharSafe(byte))
    {
      *dst++ = byte == ' ' ? '+' : byte;
    }
    else
    {
      sprintf(dst, "%%%02X", byte);
      dst += 3;
    }
  }
  *dst = '\0';
  return(encoded);
}

FormElement::FormElement (STRPTR name, STRPTR value)
{
  Name = EncodeString(name);
  Value = EncodeString(value);
}

FormElement::~FormElement ()
{
  delete Name;
  delete Value;
  delete Next;
}

ExportFormMessage::ExportFormMessage ()
{
  Elements = NULL;
  LastElement = (struct FormElement *)&Elements;

  StrLength = 0;
}

ExportFormMessage::~ExportFormMessage ()
{
  delete Elements;
}

VOID ExportFormMessage::AddElement (STRPTR name, STRPTR value)
{
  struct FormElement *element;
  element = LastElement->Next = new struct FormElement(name, value);
  LastElement = element;
  StrLength += 2 + strlen(element->Name) + strlen(element->Value);
}


VOID ExportFormMessage::GetElements (STRPTR buffer)
{
  struct FormElement *element = Elements;
  while(element)
  {
    buffer += sprintf(buffer, "%s=%s&", element->Name, element->Value);
    element = element->Next;
  }
  buffer[-1] = '\0';
}
