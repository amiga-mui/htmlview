/***************************************************************************

 Magic User Interface - MUI
 Copyright (C) 1992-2006 by Stefan Stuntz <stefan@stuntz.com>
 Copyright (C) 2006-2016 by Thore Boeckelmann, Jens Maus
 All Rights Reserved.

 This program/documents may not in whole or in part, be copied, photocopied
 reproduced, translated or reduced to any electronic medium or machine
 readable form without prior consent, in writing, from the above authors.

 With this document the authors makes no warrenties or representations,
 either expressed or implied, with respect to MUI. The information
 presented herein is being supplied on an "AS IS" basis and is expressly
 subject to change without notice. The entire risk as to the use of this
 information is assumed by the user.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 IN NO EVENT WILL THE AUTHORS BE LIABLE FOR DIRECT, INDIRECT, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES RESULTING FROM ANY CLAIM ARISING OUT OF THE
 INFORMATION PRESENTED HEREIN, EVEN IF IT HAS BEEN ADVISED OF THE
 POSSIBILITIES OF SUCH DAMAGES.

 MUI Official Support Site:  http://www.sasg.com/

 $Id: vastubs.c 5088 2016-01-02 19:52:04Z tboeckel $

***************************************************************************/

/*
   Stubs for the variable argument functions of the shared libraries used by
   MUI. Please note that these stubs should only be used if the compiler
   suite/SDK doesn't come with own stubs/inline functions.

   Also note that these stubs are only safe on m68k machines as it
   requires a linear stack layout!
*/

#if defined(__VBCC__) || defined(NO_INLINE_STDARG)
#if !defined(__PPC__)

#include <exec/types.h>

/* FIX V45 breakage... */
#if INCLUDE_VERSION < 45
#define MY_CONST_STRPTR CONST_STRPTR
#else
#define MY_CONST_STRPTR CONST STRPTR
#endif

#include <proto/dos.h>
LONG SystemTags( CONST_STRPTR command, ULONG tag1type, ... )
{ return SystemTagList(command, (struct TagItem *)&tag1type); }
struct Process *CreateNewProcTags( ULONG tag1, ... )
{ return CreateNewProc((struct TagItem *)&tag1); }

#include <proto/intuition.h>
ULONG SetAttrs( APTR object, ULONG tag1, ... )
{ return SetAttrsA(object, (struct TagItem *)&tag1); }
APTR NewObject( struct IClass *classPtr, CONST_STRPTR classID, Tag tag1, ... )
{ return NewObjectA(classPtr, classID, (struct TagItem *)&tag1); }

#include <proto/graphics.h>
LONG ObtainBestPen( struct ColorMap *cm, ULONG r, ULONG g, ULONG b, Tag tag1, ... )
{ return ObtainBestPenA(cm, r, g, b, (struct TagItem *)&tag1); }
VOID GetRPAttrs( CONST struct RastPort *rp, ULONG tag1Type, ... )
{ GetRPAttrsA(rp, (struct TagItem *)&tag1Type); }

#include <proto/datatypes.h>
Object *NewDTObject( APTR name, Tag tag1, ... )
{ return NewDTObjectA(name, (struct TagItem *)&tag1); }
ULONG SetDTAttrs( Object *o, struct Window *win, struct Requester *req, Tag tag1, ... )
{ return SetDTAttrsA(o, win, req, (struct TagItem *)&tag1); }
ULONG GetDTAttrs( Object *o, Tag tag1, ... )
{ return GetDTAttrsA(o, (struct TagItem *)&tag1); }

#else
  #error "VARGS stubs are only save on m68k systems!"
#endif // !defined(__PPC__)

#endif // defined(__VBCC__) || defined(NO_INLINE_STDARG)
