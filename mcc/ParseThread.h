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

#ifndef PARSETHREAD_H
#define PARSETHREAD_H

#include <new>
#include "SDI_compiler.h"

/* Messages */

struct ParseThreadArgs
{
  ParseThreadArgs (ULONG pageid, Object *htmlview, struct HTMLviewData *data, ULONG flags, STRPTR url, STRPTR postdata)
  {
    PageID = pageid;
    HTMLview = htmlview;
    Data = data;
    Flags = flags;
    URL = new (std::nothrow) char[strlen(url)+1];
    if (URL) strcpy(URL, url);
    PostData = postdata;

  }

  ~ParseThreadArgs ()
  {
    delete URL;
    delete PostData;
  }

  ULONG PageID;
  Object *HTMLview;
  struct HTMLviewData *Data;
  ULONG Flags;
  STRPTR URL;
  STRPTR PostData;
};


struct ParseInfoMsg
{
  struct Message nm_node;
  ULONG Class;
  ULONG Unique;
  union
  {
    struct { struct ParseMessage *PMsg; class HostClass *Object; } Startup;
    struct { struct ParseMessage *PMsg; class HostClass *Object; } Shutdown;
  };
};

#define ParseMsg_Startup  0
#define ParseMsg_Shutdown 1
#define ParseMsg_Abort    2

struct ParseThreadStartupMessage
{
	struct Message message;
	struct ParseThreadArgs *args;
};

/* Protos */

extern "C" void ParseThread(void);

#endif
