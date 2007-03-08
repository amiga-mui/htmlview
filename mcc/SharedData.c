/***************************************************************************

 HTMLview.mcc - HTMLview MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005 by TextEditor.mcc Open Source Team

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

#include <clib/alib_protos.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/muimaster.h>

#include <libraries/mui.h>

#include "Animation.h"
#include "ImageManager.h"
#include "IM_Render.h"
#include "private.h"
#include "SharedData.h"

SharedData::SharedData (struct IClass *cl, Object *obj, struct HTMLviewData *data)
{
  ENTER();

	Obj = obj;
	Data = data;

	data->Flags |= FLG_RootObj;

	/* Setup event-node */
	Events.ehn_Priority	= 0;
	Events.ehn_Flags		= 0;
	Events.ehn_Object		= obj;
	Events.ehn_Class  	= cl;
	Events.ehn_Events		= IDCMP_RAWKEY | IDCMP_MOUSEMOVE | IDCMP_MOUSEBUTTONS;

	/* Setup image-timer (for progressive decoding) */
	ImageTimer.ihn.ihn_Object	= obj;
	ImageTimer.ihn.ihn_Method	= MUIM_HTMLview_DrawImages;
	ImageTimer.ihn.ihn_Flags		= MUIIHNF_TIMER;
	ImageTimer.ihn.ihn_Millis	= 150;

	/* Setup anim-timer (for gif-anims) */
	AnimTimer.ihn.ihn_Object	= obj;
	AnimTimer.ihn.ihn_Method	= MUIM_HTMLview_AnimTick;
	AnimTimer.ihn.ihn_Flags	= MUIIHNF_TIMER;

  LEAVE();
}

VOID SharedData::Setup (struct Screen *scr)
{
  ENTER();

	Scr = scr;
	InitConfig();
//	DoMethod(_win(Obj), MUIM_Window_AddEventHandler, &Events);

  LEAVE();
}

VOID SharedData::Cleanup ()
{
  ENTER();

//	DoMethod(_win(Obj), MUIM_Window_RemEventHandler, &Events);
	FreeConfig();

	AnimTimer.Stop(Obj);
	ImageTimer.Stop(Obj);

	ULONG attempts, decoders;
	do {

		attempts = 10;
		while((decoders = DecodeQueue.DumpQueue()) && attempts--)
			Delay(10);

		if(decoders)
			MUI_Request(_app(Obj), _win(Obj), 0L, "Problems...", "Retry",
				"%ld decoder(s) are still running\n"
				"If this requester re-appears (after retry)\n"
				"then try to go offline,\n"
				"and then select retry again.", decoders);

	} while(decoders);

  LEAVE();
}

VOID SharedData::StopAnims (Object *obj)
{
  ENTER();
	
  Anims->Stop(obj, (struct AnimInfo *)&Anims);
	if(!Anims)
		AnimTimer.Stop(obj);

  LEAVE();
}

struct AnimInfo *SharedData::AddAnim (Object *obj, struct HTMLviewData *data, struct PictureFrame *picture, struct ObjectList *receivers)
{
  ENTER();

	Anims = new struct AnimInfo(obj, data, picture, receivers, Anims);

	if(AnimTimer.Stop(obj))
		AnimTimer.ihn.ihn_Millis = min(AnimTimer.ihn.ihn_Current, 10*picture->AnimDelay);
	else
    AnimTimer.ihn.ihn_Millis = 10*picture->AnimDelay;

	AnimTimer.ihn.ihn_Current = 0;

	ULONG active;
	if(get(_win(Obj), MUIA_Window_Activate, &active), active)
		AnimTimer.Start(obj);

  RETURN(Anims);
	return(Anims);
}
