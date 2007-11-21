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

 $Id: ScrollGroup.cpp 42 2007-06-10 18:06:45Z damato $

***************************************************************************/

#ifdef USEMUISTRINGS

#include <clib/alib_protos.h>
#include <clib/macros.h>
#include <libraries/mui.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include <mui/BetterString_mcc.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <string.h>

#include "General.h"
#include "StringClass.h"

extern "C" {

/*****************************************************************************************************/
/*****************************************************************************************************/

struct MUI_CustomClass *StringClass = NULL;

#ifndef MUIA_Floating
#define MUIA_Floating 0x80429753
#endif

/*
#define NewRawDoFmt(__p0, __p1, __p2, ...) \
	(((STRPTR (*)(void *, CONST_STRPTR , APTR (*)(APTR, UBYTE), STRPTR , ...))*(void**)((long)(EXEC_BASE_NAME) - 922))((void*)(EXEC_BASE_NAME), __p0, __p1, __p2, __VA_ARGS__))
*/

DISPATCHER(StringDispatcher)
{
	switch(msg->MethodID)
  	{
    	case OM_NEW:
		{
			struct opSet *nmsg = (struct opSet *)msg;
			/*struct TagItem attrs[] = {{MUIA_Floating, TRUE},
						              {TAG_MORE, (ULONG)nmsg->ops_AttrList}{;

		    obj = (Object *)DoSuperMethod(cl,obj,OM_NEW,attrs,nmsg->ops_GInfo);*/

		    obj = (Object *)DoSuperMethodA(cl,obj,msg);
            if (obj)
		    {
				struct StringData *data = (struct StringData *)INST_DATA(cl,obj);

				data->Columns = GetTagData(MUIA_BetterString_Columns,0,nmsg->ops_AttrList);
			}

		    return (ULONG)obj;
    	}

		case MUIM_AskMinMax:
        {
			struct StringData     *data = (struct StringData *)INST_DATA(cl,obj);
			struct MUIP_AskMinMax *amsg = (struct MUIP_AskMinMax *)msg;

		    DoSuperMethodA(cl,obj,(Msg)msg);

            if (data->Columns>0)
            {
			    WORD w;

			    SetFont(&data->rp,muiAreaData(obj)->mad_Font);
				w = data->Columns*TextLength(&data->rp,"n",1);

                /*NewRawDoFmt("W:%ld Col:%ld %ld %ld %ld\n",(void* (*)(void*,UBYTE))1,NULL,
                	w,
                    data->Columns,
				    amsg->MinMaxInfo->MinWidth,
				    amsg->MinMaxInfo->DefWidth,
				    amsg->MinMaxInfo->MaxWidth);*/

			    amsg->MinMaxInfo->DefWidth += w;
                amsg->MinMaxInfo->MinWidth += w; //= amsg->MinMaxInfo->DefWidth;
			    amsg->MinMaxInfo->MaxWidth += w; //amsg->MinMaxInfo->DefWidth;
			}

        	return 0;
		}

		case MUIM_Setup:
		{
			struct StringData *data = (struct StringData *)INST_DATA(cl,obj);

		    if (!DoSuperMethodA(cl,obj,(Msg)msg)) return FALSE;
			memcpy(&data->rp,&_screen(obj)->RastPort,sizeof(data->rp));

			return TRUE;
		}

    	default:
	        break;
    }

	return DoSuperMethodA(cl,obj,(APTR)msg);
}

/*****************************************************************************************************/
/*****************************************************************************************************/

} // extern "C"

#endif
