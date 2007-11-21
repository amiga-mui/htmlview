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
#include <string.h>
#include <proto/dos.h>
#include <proto/exec.h>

#include "General.h"
#include "TagInfo.h"
#include "ParseMessage.h"
#include "ParseThread.h"
#include "Entities.h"
#include "ScanArgs.h"
#include "Classes.h"

#include "classes/HostClass.h"

#include "private.h"

//#define OUTPUT
//#define TIME_IT

/*extern "C" ULONG StackReg ();
ULONG StackUsage = 0;
VOID StackCheck ()
{
  StackUsage = max(StackUsage, (ULONG)FindTask(NULL)->tc_SPUpper-StackReg());
}*/

static struct SignalSemaphore mutex;

/*extern "C" VOID _INIT_5_ParseThreadSemaphore();
VOID _INIT_5_ParseThreadSemaphore()
{
  memset(&mutex, 0, sizeof(struct SignalSemaphore));
  InitSemaphore(&mutex);
}*/


#ifdef OUTPUT
VOID PrintTag (STRPTR tag)
{
  STRPTR tagend = tag;

  while(!TagEnd(*tagend))
    tagend++;

  STRPTR newtag = new UBYTE [(tagend-tag)+1];
  strncpy(newtag, tag, tagend-tag);
  newtag[tagend-tag] = '\0';
  printf("%s>\n", newtag);
  delete newtag;
}
#endif

//#undef OUTPUT

//VOID ParseThread(REG(a0, STRPTR arguments ))
VOID ParseThread(REG(a0,STRPTR arguments))
{
  //if(sscanf(arguments, "%x", (unsigned int *)&args))
  if(1)
  {
    struct ParseThreadArgs *args = (struct ParseThreadArgs *)arguments;

    struct Hook *loadhook = args->Data->LoadHook;

    struct MsgPort *myport;
    if((myport = CreateMsgPort()))
    {
      struct ParseInfoMsg msg;
      msg.nm_node.mn_ReplyPort = myport;
      msg.nm_node.mn_Length = sizeof(struct ParseInfoMsg);
      msg.Unique = args->PageID;

      STRPTR buf = new char[2048];
      struct ParseMessage pmsg(buf, 2048, loadhook, args->PageID);

      ULONG flags = 0L;
      if(args->Flags & FLG_Reload)
        flags |= MUIF_HTMLview_LoadMsg_Reload;
      if(args->PostData)
        flags |= MUIF_HTMLview_LoadMsg_Post;

      /* This is sort of a hack... */
      if(args->PostData)
      {
        pmsg.LoadMsg.PostLength = strlen(args->PostData);
        pmsg.LoadMsg.EncodingType = args->Data->EncodingType;
      }

      if(pmsg.OpenURL(args->URL, args->HTMLview, flags))
      {
        pmsg.WriteURL(args->PostData);

        if(pmsg.Fetch(32) && *pmsg.Current != '<')
          pmsg.NextStartBracket();

        /*ObtainSemaphore(&mutex);
		kprintf("PARSETHREAD %lx FOR OBJ %lx MSGPORT %lx ARGS %lx\n",FindTask(NULL),args->HTMLview,args->Data->MessagePort,args);
        ReleaseSemaphore(&mutex);*/

        class HostClass *obj = new class HostClass(args->HTMLview, args->Data);
        msg.Class = ParseMsg_Startup;
        msg.Startup.PMsg = &pmsg;
        msg.Startup.Object = obj;
/*        ObtainSemaphore(&mutex);
        printf("0x%lx: Sending startup msg (0x%lx, 0x%lx)\n",  FindTask(NULL), obj, &pmsg);
        ReleaseSemaphore(&mutex);
*/        PutMsg(args->Data->MessagePort, &msg.nm_node);
        WaitPort(myport);
        GetMsg(myport);

        pmsg.Host = obj;
#ifdef TIME_IT
        struct DateStamp t1, t2;
        DateStamp(&t1);
#endif
        obj->Parse(pmsg);
#ifdef TIME_IT
        DateStamp(&t2);
        LONG ticks = t2.ds_Tick - t1.ds_Tick;
        if(ticks < 0)
          ticks += TICKS_PER_SECOND * 60;
        kprintf("Time: %ld\n", ticks);
#endif
        pmsg.NextStartBracket();
        obj->setFlags(obj->flags() | FLG_AllElementsPresent);

        msg.Class = ParseMsg_Shutdown;
        msg.Shutdown.PMsg = &pmsg;
        msg.Shutdown.Object = obj;

/*        ObtainSemaphore(&mutex);
        printf("0x%lx: Sending shutdown msg (0x%lx, 0x%lx)\n",  FindTask(NULL), obj, &pmsg);
        ReleaseSemaphore(&mutex);
*/        PutMsg(args->Data->MessagePort, &msg.nm_node);
        WaitPort(myport);
        GetMsg(myport);

        pmsg.CloseURL();
      }
      else
      {
        msg.Class = ParseMsg_Abort;
        PutMsg(args->Data->MessagePort, &msg.nm_node);
        WaitPort(myport);
        GetMsg(myport);
      }
      DeleteMsgPort(myport);
    }

    delete args;
  }
//  kprintf("ParseStack: %ld\n", StackUsage);
}
