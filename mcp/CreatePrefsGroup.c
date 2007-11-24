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

#include <string.h>
#include <stdio.h>

#include <clib/alib_protos.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/locale.h>
#include <proto/utility.h>
#include <exec/libraries.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#ifdef USEBETTERSTRING
#include <mui/BetterString_mcc.h>
#endif
#ifdef USEHOTKEY
#include <mui/HotkeyString_mcc.h>
#endif
#include <workbench/startup.h>
#include <workbench/workbench.h>

#include "muiextra.h"
#include "HTMLview_mcc.h"
#include "private.h"
#include "rev.h"
#include "ScrollGroup.h"
#include "locale.h"

/***********************************************************************/

#define _HELP(h) ((ULONG)(h)) ? MUIA_ShortHelp   : TAG_IGNORE, ((ULONG)(h)) ? (ULONG)GetStr((APTR)(h)) : 0

/***********************************************************************/

char GetKeyChar(APTR id,BOOL lower)
{
	UBYTE *s, k;

	if ((k = (id && (s = (UBYTE *)GetStr(id))) ? *s : 0))
    {
		if (lower && LocaleBase)
	    	return ToLower(k);
    }

	return k;
}

/***********************************************************************/

Object *
ostring(APTR key,APTR help)
{
    #ifdef USEBETTERSTRING
    return BetterStringObject,
    #else
    return StringObject,
    #endif
        _HELP(help),
		MUIA_ControlChar, GetKeyChar(key,TRUE),
        MUIA_CycleChain,  TRUE,
        StringFrame,
        //MUIA_Textinput_AdvanceOnCR, TRUE,
    End;
}

/***********************************************************************/

Object *
ocheck(APTR key,APTR help)
{
    Object *obj;

    if (obj = MUI_MakeObject(MUIO_Checkmark,NULL))
        SetAttrs(obj,MUIA_CycleChain,TRUE,MUIA_ControlChar,GetKeyChar(key,TRUE),_HELP(help),TAG_DONE);

    return obj;
}

/***********************************************************************/

Object *
olabel1(APTR id,APTR key)
{
	return MUI_MakeObject(MUIO_Label,(ULONG)GetStr(id),MUIO_Label_SingleFrame|GetKeyChar(key,FALSE));
}


/****************************************************************************/

Object *
olabel2(APTR id,APTR key)
{
	return MUI_MakeObject(MUIO_Label,(ULONG)GetStr(id),MUIO_Label_DoubleFrame|GetKeyChar(key,FALSE));
}

/***********************************************************************/

Object *
otclabel(APTR id,APTR key)
{
	return MUI_MakeObject(MUIO_Label,(ULONG)GetStr(id),MUIO_Label_Centered|MUIO_Label_Tiny|GetKeyChar(key,FALSE));
}

/***********************************************************************/

Object *
obutton(APTR text,APTR key,APTR help)
{
    register Object *obj;

    if (obj = MUI_MakeObject(MUIO_Button,(ULONG)GetStr(text)))
    {
        UBYTE k = GetKeyChar(key,FALSE), kl = GetKeyChar(key,TRUE);
        
        SetAttrs(obj,MUIA_CycleChain,TRUE,MUIA_ControlChar,kl,MUIA_Text_ControlChar,kl,_HELP(help),TAG_DONE);
	}

    return obj;
}

/***********************************************************************/

Object *
opoppen(APTR key,APTR title,APTR help)
{
    return PoppenObject,
        MUIA_ControlChar,  GetKeyChar(key,TRUE),
        MUIA_Draggable,    TRUE,
        MUIA_CycleChain,   TRUE,
        _HELP(help),
    End;
}

/***********************************************************************/

Object *CreatePrefsGroup(Object *parent,struct InstData_MCP *data)
{
    ULONG PopImg = MUIMasterBase->lib_Version < 20 ? MUII_PopUp : MUII_PopFont;
    static const char *PageTitles[5];
    static const char *Dither[3];

    Object **objs = data->Objects;
    Object *group, *sample;
    #ifdef USEHOTKEY
    Object *snoop;
    #endif

	PageTitles[0] = GetStr(MSG_Page_Fonts);
    PageTitles[1] = GetStr(MSG_Page_Graphics);
    PageTitles[2] = GetStr(MSG_Page_Settings);
    PageTitles[3] = GetStr(MSG_Page_Sample);
    PageTitles[4] = NULL;

    Dither[0] = GetStr(MSG_Images_Dither_FS);
    Dither[1] = GetStr(MSG_Images_Dither_None);
    Dither[2] = NULL;

    group = RegisterObject,
        MUIA_CycleChain, TRUE,
        MUIA_Register_Titles, PageTitles,

        /* Fonts */
        Child, VGroup,

            //Child, RectangleObject, End,

            Child, HGroup,
                Child, ColGroup(4),
                    Child, olabel2(MSG_Fonts_Normal,MSG_Fonts_Normal_Key),
                    Child, PopaslObject,
                        MUIA_Popstring_String, objs[NormalFont] = ostring(MSG_Fonts_Normal_Key,0),
                        MUIA_Popstring_Button, PopButton(PopImg),
                        MUIA_Popasl_Type, ASL_FontRequest,
                        ASLFO_MaxHeight,  98,
                        End,
                    Child, olabel2(MSG_Fonts_Small,MSG_Fonts_Small_Key),
                    Child, PopaslObject,
                        MUIA_Popstring_String, objs[SmallFont] = ostring(MSG_Fonts_Small_Key,0),
                        MUIA_Popstring_Button, PopButton(PopImg),
                        MUIA_Popasl_Type, ASL_FontRequest,
                        ASLFO_MaxHeight,  98,
                        End,
                    Child, olabel2(MSG_Fonts_Fixed,MSG_Fonts_Fixed_Key),
                    Child, PopaslObject,
                        MUIA_Popstring_String, objs[FixedFont] = ostring(MSG_Fonts_Fixed_Key,0),
                        MUIA_Popstring_Button, PopButton(PopImg),
                        MUIA_Popasl_Type, ASL_FontRequest,
                        ASLFO_Flags,      FOF_FIXEDWIDTHONLY,
                        ASLFO_MaxHeight,  98,
                        End,
                    Child, olabel2(MSG_Fonts_Large,MSG_Fonts_Large_Key),
                    Child, PopaslObject,
                        MUIA_Popstring_String, objs[LargeFont] = ostring(MSG_Fonts_Large_Key,0),
                        MUIA_Popstring_Button, PopButton(PopImg),
                        MUIA_Popasl_Type, ASL_FontRequest,
                        ASLFO_MaxHeight,  98,
                        End,

                    /*Child, RectangleObject, MUIA_Height, 10, End,
                    Child, RectangleObject, MUIA_Height, 10, End,
                    Child, RectangleObject, MUIA_Height, 10, End,
                    Child, RectangleObject, MUIA_Height, 10, End,*/

                    Child, olabel2(MSG_Fonts_H1,MSG_Fonts_H1_Key),
                    Child, PopaslObject,
                        MUIA_Popstring_String, objs[H1] = ostring(MSG_Fonts_H1_Key,0),
                        MUIA_Popstring_Button, PopButton(PopImg),
                        MUIA_Popasl_Type, ASL_FontRequest,
                        ASLFO_MaxHeight,  98,
                        End,

                    Child, olabel2(MSG_Fonts_H4,MSG_Fonts_H4_Key),
                    Child, PopaslObject,
                        MUIA_Popstring_String, objs[H4] = ostring(MSG_Fonts_H4_Key,0),
                        MUIA_Popstring_Button, PopButton(PopImg),
                        MUIA_Popasl_Type, ASL_FontRequest,
                        ASLFO_MaxHeight,  98,
                        End,

                    Child, olabel2(MSG_Fonts_H2,MSG_Fonts_H2_Key),
                    Child, PopaslObject,
                        MUIA_Popstring_String, objs[H2] = ostring(MSG_Fonts_H2_Key,0),
                        MUIA_Popstring_Button, PopButton(PopImg),
                        MUIA_Popasl_Type, ASL_FontRequest,
                        ASLFO_MaxHeight,  98,
                        End,

                    Child, olabel2(MSG_Fonts_H5,MSG_Fonts_H5_Key),
                    Child, PopaslObject,
                        MUIA_Popstring_String, objs[H5] = ostring(MSG_Fonts_H5_Key,0),
                        MUIA_Popstring_Button, PopButton(PopImg),
                        MUIA_Popasl_Type, ASL_FontRequest,
                        ASLFO_MaxHeight,  98,
                        End,

                    Child, olabel2(MSG_Fonts_H3,MSG_Fonts_H3_Key),
                    Child, PopaslObject,
                        MUIA_Popstring_String, objs[H3] = ostring(MSG_Fonts_H3_Key,0),
                        MUIA_Popstring_Button, PopButton(PopImg),
                        MUIA_Popasl_Type, ASL_FontRequest,
                        ASLFO_MaxHeight,  98,
                        End,

                    Child, olabel2(MSG_Fonts_H6,MSG_Fonts_H6_Key),
                    Child, PopaslObject,
                        MUIA_Popstring_String, objs[H6] = ostring(MSG_Fonts_H6_Key,0),
                        MUIA_Popstring_Button, PopButton(PopImg),
                        MUIA_Popasl_Type, ASL_FontRequest,
                        ASLFO_MaxHeight,  98,
                        End,
                    End,
                End,

            Child, RectangleObject, End,

            End,

        /* Graphics */
        Child, VGroup,

            Child, VGroup,
                GroupFrameT(GetStr(MSG_Colours_Title)),

                Child, ColGroup(5),
                    MUIA_Group_SameSize,TRUE,
                    Child, objs[Col_Background] = opoppen(MSG_Colours_Backgound,MSG_Colours_Backgound_Key,0),
                    Child, objs[Col_Text] = opoppen(MSG_Colours_Text,MSG_Colours_Text_Key,0),
                    Child, objs[Col_Link] = opoppen(MSG_Colours_NormalLink,MSG_Colours_NormalLink_Key,0),
                    Child, objs[Col_VLink] = opoppen(MSG_Colours_VisitedLink,MSG_Colours_VisitedLink_Key,0),
                    Child, objs[Col_ALink] = opoppen(MSG_Colours_SelectedLink,MSG_Colours_SelectedLink_Key,0),

                    Child, otclabel(MSG_Colours_Backgound,MSG_Colours_Backgound_Key),
                    Child, otclabel(MSG_Colours_Text,MSG_Colours_Text_Key),
                    Child, otclabel(MSG_Colours_NormalLink,MSG_Colours_NormalLink_Key),
                    Child, otclabel(MSG_Colours_VisitedLink,MSG_Colours_VisitedLink_Key),
                    Child, otclabel(MSG_Colours_SelectedLink,MSG_Colours_SelectedLink_Key),
                    End,

                Child, HGroup,
                    Child, RectangleObject, End,
                    Child, objs[IgnoreDocCols] = ocheck(MSG_Colours_IgnoreColours,MSG_Colours_IgnoreColours_Key),
                    Child, olabel1(MSG_Colours_IgnoreColours,MSG_Colours_IgnoreColours_Key),
                    Child, RectangleObject, End,
                    End,

                End,

            Child, VGroup,
                GroupFrameT(GetStr(MSG_Images_Title)),
                //Child, RectangleObject, MUIA_Weight, 50, End,

                Child, ColGroup(2),

                    Child, olabel2(MSG_Images_Listitems,MSG_Images_Listitems_Key),
                    Child, PopaslObject,
                        MUIA_Popstring_String, objs[ListItemFile] = ostring(MSG_Images_Listitems_Key,0),
                        MUIA_Popstring_Button, PopButton(MUII_PopFile),
                        MUIA_Popasl_Type, ASL_FileRequest,
                        End,

                    Child, olabel2(MSG_Images_Dither,MSG_Images_Dither_Key),
                    Child, objs[DitherType] = CycleObject,
                        MUIA_ControlChar,   GetKeyChar(MSG_Images_Dither_Key,TRUE),
                        MUIA_CycleChain,    TRUE,
                        MUIA_Cycle_Entries, Dither,
                        End,

                    Child, olabel2(MSG_Images_CacheSize,MSG_Images_CacheSize_Key),
                    Child, objs[ImageCacheSize] = (Object *)NewObject(CacheSliderClass->mcc_Class, NULL,
                        MUIA_ControlChar, GetKeyChar(MSG_Images_CacheSize_Key,TRUE),
                        MUIA_CycleChain,  TRUE,
                        MUIA_Numeric_Min, 1,
                        MUIA_Numeric_Max, 1024*8,
                        End,

                    Child, olabel2(MSG_Images_Gamma,MSG_Images_Gamma_Key),
                    Child, objs[GammaCorrection] = (Object *)NewObject(GammaSliderClass->mcc_Class, NULL,
                        MUIA_ControlChar, GetKeyChar(MSG_Images_Gamma_Key,TRUE),
                        MUIA_CycleChain, TRUE,
                        MUIA_Numeric_Min, 1000,
                        MUIA_Numeric_Max, 9000,
                        End,

                    End,

                    Child, RectangleObject, MUIA_Weight, 50, End,
                End,

                Child, RectangleObject, End,

            End,

        /* Settings */
        Child, VGroup,

            Child, VGroup,
                GroupFrameT(GetStr(MSG_PageScroll_Title)),

                Child, ColGroup(2),

                    Child, olabel2(MSG_PageScroll_Smooth,MSG_PageScroll_Smooth_Key),
                    Child, HGroup,
                        Child, objs[PageScrollSmooth] = ocheck(MSG_PageScroll_Smooth,MSG_PageScroll_Smooth_Key),
                        Child, RectangleObject, End,
                        End,

                    Child, olabel2(MSG_PageScroll_Key,MSG_PageScroll_Key_Key),
                    #ifdef USEHOTKEY
                    Child, HGroup,
                    	MUIA_Group_HorizSpacing,1,
                        Child, objs[PageScrollKey] = HotkeyStringObject,
                            StringFrame,
                            MUIA_ControlChar, GetKeyChar(MSG_PageScroll_Key_Key,TRUE),
                            MUIA_CycleChain,  TRUE,
                            MUIA_HotkeyString_Snoop, FALSE,
                            End,
                        Child, snoop = obutton(MSG_Hotkey_Snoop,0,0),
                        End,
                    #else
                    Child, objs[PageScrollKey] = KeyadjustObject,
                        MUIA_ControlChar, GetKeyChar(MSG_PageScroll_Key_Key,TRUE),
                        MUIA_CycleChain, TRUE,
                        MUIA_Keyadjust_AllowMouseEvents,  FALSE,
                        MUIA_Keyadjust_AllowMultipleKeys, FALSE,
                        End,
                    #endif

                    Child, olabel2(MSG_PageScroll_Move,MSG_PageScroll_Move_Key),
                    Child, objs[PageScrollMove] = SliderObject,
                        MUIA_ControlChar, GetKeyChar(MSG_PageScroll_Move_Key,TRUE),
                        MUIA_CycleChain, TRUE,
                        MUIA_Numeric_Min, 1,
                        MUIA_Numeric_Max, 100,
                        MUIA_Numeric_Format, GetStr(MSG_PageScroll_Move_Down),
                        End,

                    End,

                End,

            Child, RectangleObject, End,

            /*Child, TextObject,
                TextFrame,
                MUIA_Background, MUII_TextBack,
                MUIA_Text_Contents, "\33cNot many settings can be found on this page.\nPlease write and suggest something!",
                End,*/

            End,

        Child, data->SampleGroup = VGroup,
        	Child,  data->FirstSample = VGroup,
	        	Child, VSpace(0),
    	        Child, HGroup,
        	    	Child, RectangleObject, MUIA_Weight, 200, End,
		            Child, sample = obutton(MSG_SampleButton,MSG_SampleButton_Key,0),
    		        Child, RectangleObject, MUIA_Weight, 200, End,
	    		    End,
                Child, VSpace(0),
        		End,
            End,
        End;

    #ifdef USEHOTKEY
    SetAttrs(snoop,
        MUIA_Weight, 0,
        MUIA_CycleChain, TRUE,
        MUIA_InputMode, MUIV_InputMode_Toggle,
        TAG_DONE);

    DoMethod(snoop, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, (ULONG)objs[PageScrollKey], 3, MUIM_Set, MUIA_HotkeyString_Snoop, MUIV_TriggerValue);
    DoMethod(snoop, MUIM_Notify, MUIA_Selected, TRUE, MUIV_Notify_Window, 3, MUIM_Set, MUIA_Window_ActiveObject, (ULONG)objs[PageScrollKey]);
    #endif

    DoMethod(sample,MUIM_Notify,MUIA_Pressed,FALSE,MUIV_Notify_Application,4,
    	MUIM_Application_PushMethod,(ULONG)parent,1,MM_HTMLview_CreateSample);

    return group;
}

