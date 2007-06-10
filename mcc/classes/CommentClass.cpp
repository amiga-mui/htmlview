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

#include "CommentClass.h"

#include "ParseMessage.h"

VOID CommentClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
  BOOL out_of_comment = FALSE;
  pmsg.SetLock(); /* we don't want to buffer the entire comment */
  pmsg.Current += 4; /* skip '<!--' */
  pmsg.Current[-1] = '_';

  do {

    while(pmsg.Fetch() && *pmsg.Current != '-' && *pmsg.Current != '>')
      pmsg.Current++;

    if(*pmsg.Current == '-' && pmsg.Current[-1] == '-')
    {
      *pmsg.Current = '_';
      out_of_comment = TRUE; /* Should be a toggle */
    }
    else if(*pmsg.Current == '>' && out_of_comment)
    {
      pmsg.Current++;
      break;
    }
    pmsg.Current++;

  } while(pmsg.Fetch());

#ifdef OUTPUT
  printf("<!-- -->\n");
#endif
}
