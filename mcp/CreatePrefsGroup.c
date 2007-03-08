/***************************************************************************

 HTMLview.mcc - HTMLview MUI Custom Class
 Copyright (C) 1997-2000 Allan Odgaard
 Copyright (C) 2005 by HTMLview.mcc Open Source Team

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
#include <exec/libraries.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <mui/BetterString_mcc.h>
#include <mui/HotkeyString_mcc.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>

#include "muiextra.h"

#include "HTMLview_mcc.h"

#include "private.h"
#include "rev.h"

#include "ScrollGroup.h"

struct MUIP_AppMessage { struct AppMessage *appmsg; };

HOOKPROTONH(AppMessageCode, VOID, Object* htmlview, struct MUIP_AppMessage *args)
{
	if(args->appmsg->am_NumArgs)
	{
		UBYTE dir [256+8];
		strcpy(dir, "file://");
		if(NameFromLock(args->appmsg->am_ArgList[0].wa_Lock, dir+7, 256))
		{
			AddPart(dir+7, args->appmsg->am_ArgList[0].wa_Name, 256);
			DoMethod(htmlview, MUIM_HTMLview_GotoURL, dir, NULL);
		}
	}
}
MakeStaticHook(AppMessageHook, AppMessageCode);

Object *StringGadget(UBYTE ctrlchar, BOOL dragable)
{
	return BetterStringObject,
		StringFrame,
		MUIA_ControlChar,				  ctrlchar,
		MUIA_CycleChain,				  TRUE,
		MUIA_String_AdvanceOnCR,	TRUE,
		MUIA_Draggable,				    dragable,
		End;
}

Object *CheckMarkGadget(UBYTE ctrlchar)
{
	return ImageObject,
		ImageButtonFrame,
		MUIA_ControlChar,		ctrlchar,
		MUIA_CycleChain,		TRUE,
		MUIA_InputMode,		  MUIV_InputMode_Toggle,
		MUIA_Image_Spec,		MUII_CheckMark,
		MUIA_Image_FreeVert,TRUE,
		MUIA_Background,		MUII_ButtonBack,
		MUIA_ShowSelState,	FALSE,
		End;
}

Object *MyLabel (STRPTR label, UWORD frame, UBYTE key)
{
	return MUI_MakeObject(MUIO_Label, label, MUIO_Label_DontCopy | frame | key);
}

VOID CreateSampleText(Object *htmlview)
{
	ULONG ver, rev;
	get(htmlview, MUIA_Version, &ver);
	get(htmlview, MUIA_Revision, &rev);
	STRPTR extra = MUIMasterBase->lib_Version > 19 ? "<Br><Br><P Align=Center><Small>You can drop HTML files onto this gadget!</Small>" : "";

	UBYTE text[800];
	sprintf(text, "<Body>"

		"<Table CellSpacing=0 CellPadding=0 Align=Center>"
		"<TR><TD RowSpan=2 VAlign=Bottom><Font Size=+2>HTMLview&nbsp;"
		"<TD><Small>.mcc V%ld.%ld"
		"<TR><TD><Small>.mcp V%d.%d"
		"<TR><TD Height=4 ColSpan=2>"
		"</Table>"
		"<Div Align=Center><Font Size=+1>Copyright 1997-2000 by Allan Odgaard<Br>"
    "Copyright 2005 HTMLview.mcc Open Source Team<br>"
		"Image decoders by Gunther Nikl"
		"<Hr></Div>"

		"<P>For the latest version, try: <a href=http://www.sourceforge.net/projects/htmlview-mcc/>http://www.sourceforge.net/projects/htmlview-mcc/</a><br>"
		"%s\n",

	  ver, rev,
		LIB_VERSION, LIB_REVISION,
		extra);

	set(htmlview, MUIA_HTMLview_Contents, text);
}

Object *CreatePrefsGroup(struct InstData_MCP *data)
{
	ULONG PopImg = MUIMasterBase->lib_Version < 20 ? MUII_PopUp : MUII_PopFont;
	static STRPTR PageTitles[] = { "Fonts", "Graphics", "Settings", "Sample", NULL };
	static STRPTR Dither[] = { "Floyd steinberg", "None", NULL };

	Object **objs = data->Objects;
	Object *group, *snoop, *htmlview;

	group = RegisterObject,
		MUIA_CycleChain, TRUE,
		MUIA_Register_Titles, PageTitles,

		/* Fonts */
		Child, VGroup,

			Child, RectangleObject, End,

			Child, HGroup,
				Child, ColGroup(4),
					Child, MyLabel("Normal", MUIO_Label_DoubleFrame, 'N'),
					Child, PopaslObject,
						MUIA_Popstring_String,	objs[NormalFont] = StringGadget('n', TRUE),
						MUIA_Popstring_Button,	PopButton(PopImg),
						MUIA_Popasl_Type,			ASL_FontRequest,
						ASLFO_MaxHeight,			98,
						End,
					Child, MyLabel("Small", MUIO_Label_DoubleFrame, 'S'),
					Child, PopaslObject,
						MUIA_Popstring_String,	objs[SmallFont] = StringGadget('s', TRUE),
						MUIA_Popstring_Button,	PopButton(PopImg),
						MUIA_Popasl_Type,			ASL_FontRequest,
						ASLFO_MaxHeight,			98,
						End,
					Child, MyLabel("Fixed", MUIO_Label_DoubleFrame, 'F'),
					Child, PopaslObject,
						MUIA_Popstring_String,	objs[FixedFont] = StringGadget('f', TRUE),
						MUIA_Popstring_Button,	PopButton(PopImg),
						MUIA_Popasl_Type,			ASL_FontRequest,
						ASLFO_Flags,				FOF_FIXEDWIDTHONLY,
						ASLFO_MaxHeight,			98,
						End,
					Child, MyLabel("Large", MUIO_Label_DoubleFrame, 'L'),
					Child, PopaslObject,
						MUIA_Popstring_String,	objs[LargeFont] = StringGadget('l', TRUE),
						MUIA_Popstring_Button,	PopButton(PopImg),
						MUIA_Popasl_Type,			ASL_FontRequest,
						ASLFO_MaxHeight,			98,
						End,

					Child, RectangleObject, MUIA_Height, 10, End,
					Child, RectangleObject, MUIA_Height, 10, End,
					Child, RectangleObject, MUIA_Height, 10, End,
					Child, RectangleObject, MUIA_Height, 10, End,

					Child, MyLabel("H1", MUIO_Label_DoubleFrame, '1'),
					Child, PopaslObject,
						MUIA_Popstring_String,	objs[H1] = StringGadget('1', TRUE),
						MUIA_Popstring_Button,	PopButton(PopImg),
						MUIA_Popasl_Type,			ASL_FontRequest,
						ASLFO_MaxHeight,			98,
						End,
					Child, MyLabel("H4", MUIO_Label_DoubleFrame, '4'),
					Child, PopaslObject,
						MUIA_Popstring_String,	objs[H4] = StringGadget('4', TRUE),
						MUIA_Popstring_Button,	PopButton(PopImg),
						MUIA_Popasl_Type,			ASL_FontRequest,
						ASLFO_MaxHeight,			98,
						End,
					Child, MyLabel("H2", MUIO_Label_DoubleFrame, '2'),
					Child, PopaslObject,
						MUIA_Popstring_String,	objs[H2] = StringGadget('2', TRUE),
						MUIA_Popstring_Button,	PopButton(PopImg),
						MUIA_Popasl_Type,			ASL_FontRequest,
						ASLFO_MaxHeight,			98,
						End,
					Child, MyLabel("H5", MUIO_Label_DoubleFrame, '5'),
					Child, PopaslObject,
						MUIA_Popstring_String,	objs[H5] = StringGadget('5', TRUE),
						MUIA_Popstring_Button,	PopButton(PopImg),
						MUIA_Popasl_Type,			ASL_FontRequest,
						ASLFO_MaxHeight,			98,
						End,
					Child, MyLabel("H3", MUIO_Label_DoubleFrame, '3'),
					Child, PopaslObject,
						MUIA_Popstring_String,	objs[H3] = StringGadget('3', TRUE),
						MUIA_Popstring_Button,	PopButton(PopImg),
						MUIA_Popasl_Type,			ASL_FontRequest,
						ASLFO_MaxHeight,			98,
						End,
					Child, MyLabel("H6", MUIO_Label_DoubleFrame, '6'),
					Child, PopaslObject,
						MUIA_Popstring_String,	objs[H6] = StringGadget('6', TRUE),
						MUIA_Popstring_Button,	PopButton(PopImg),
						MUIA_Popasl_Type,			ASL_FontRequest,
						ASLFO_MaxHeight,			98,
						End,
					End,
				End,

			Child, RectangleObject, End,

			End,

		/* Graphics */
		Child, VGroup,

			Child, VGroup,
				GroupFrameT("Colours"),

				Child, HGroup,
					Child, RectangleObject, End,
					Child, MyLabel("Ignore document provided colours:", MUIO_Label_SingleFrame, 'I'),
					Child, objs[IgnoreDocCols] = CheckMarkGadget('i'),
					End,

				Child, ColGroup(5),
					Child, objs[Col_Background] = PoppenObject, MUIA_ControlChar, 'b', MUIA_CycleChain, TRUE, End,
					Child, objs[Col_Text] = PoppenObject, MUIA_ControlChar, 't', MUIA_CycleChain, TRUE, End,
					Child, objs[Col_Link] = PoppenObject, MUIA_ControlChar, 'n', MUIA_CycleChain, TRUE, End,
					Child, objs[Col_VLink] = PoppenObject, MUIA_ControlChar, 'v', MUIA_CycleChain, TRUE, End,
					Child, objs[Col_ALink] = PoppenObject, MUIA_ControlChar, 's', MUIA_CycleChain, TRUE, End,
					Child, MyLabel("Background", MUIO_Label_Centered | MUIO_Label_Tiny, 'B'),
					Child, MyLabel("Text", MUIO_Label_Centered | MUIO_Label_Tiny, 'T'),
					Child, MyLabel("Normal Link", MUIO_Label_Centered | MUIO_Label_Tiny, 'N'),
					Child, MyLabel("Visited link", MUIO_Label_Centered | MUIO_Label_Tiny, 'V'),
					Child, MyLabel("Selected link", MUIO_Label_Centered | MUIO_Label_Tiny, 'S'),
					End,

				End,

			Child, HGroup,
				GroupFrameT("ListItem Images"),
				Child, MyLabel("File", MUIO_Label_DoubleFrame, 'F'),
				Child, PopaslObject,
					MUIA_Popstring_String,	objs[ListItemFile] = StringGadget('f', FALSE),
					MUIA_Popstring_Button,	PopButton(MUII_PopFile),
					MUIA_Popasl_Type,			ASL_FileRequest,
					End,

				End,

			Child, VGroup,
				GroupFrameT("Images"),
				Child, RectangleObject, MUIA_Weight, 50, End,

				Child, ColGroup(2),

					Child, MyLabel("Dither", MUIO_Label_SingleFrame, 'D'),
					Child, objs[DitherType] = CycleObject,
						MUIA_ControlChar, 'd',
						MUIA_CycleChain, TRUE,
						MUIA_Cycle_Entries, Dither,
						End,

					Child, MyLabel("Cache size", MUIO_Label_DoubleFrame, 'C'),
					Child, objs[ImageCacheSize] = (Object *)NewObject(CacheSliderClass->mcc_Class, NULL,
						MUIA_ControlChar, 'c',
						MUIA_CycleChain, TRUE,
						MUIA_Numeric_Min, 1,
						MUIA_Numeric_Max, 1024*8,
						End,

					Child, MyLabel("Gamma correction", MUIO_Label_DoubleFrame, 'G'),
					Child, objs[GammaCorrection] = (Object *)NewObject(GammaSliderClass->mcc_Class, NULL,
						MUIA_ControlChar, 'g',
						MUIA_CycleChain, TRUE,
						MUIA_Numeric_Min, 1000,
						MUIA_Numeric_Max, 9000,
						End,

					End,

				Child, RectangleObject, MUIA_Weight, 50, End,
				End,

			End,

		/* Settings */
		Child, VGroup,

			Child, VGroup,
				GroupFrameT("Page scroll"),

				Child, ColGroup(2),

					Child, MyLabel("Smooth", MUIO_Label_SingleFrame, 'S'),
					Child, HGroup,
						Child, objs[PageScrollSmooth] = CheckMarkGadget('s'),
						Child, RectangleObject, End,
						End,

					Child, MyLabel("Key", MUIO_Label_SingleFrame, 'K'),
					Child, HGroup,
						Child, objs[PageScrollKey] = HotkeyStringObject,
							StringFrame,
							MUIA_ControlChar, 'k',
							MUIA_CycleChain, TRUE,
							MUIA_HotkeyString_Snoop, FALSE,
							End,
						Child, snoop = SimpleButton("Snoop"),
						End,

					Child, MyLabel("Move", MUIO_Label_DoubleFrame, 'M'),
					Child, objs[PageScrollMove] = SliderObject,
						MUIA_ControlChar, 'm',
						MUIA_CycleChain, TRUE,
						MUIA_Numeric_Min, 1,
						MUIA_Numeric_Max, 100,
						MUIA_Numeric_Format, "%ld%% down",
						End,

					End,

				End,

			Child, RectangleObject, End,

			Child, TextObject,
				TextFrame,
				MUIA_Background, MUII_TextBack,
				MUIA_Text_Contents, "\33cNot many settings can be found on this page.\nPlease write and suggest something!",
				End,

			End,

		/* Sample */
		Child, htmlview = HTMLviewObject,
			VirtualFrame,
			MUIA_CycleChain, TRUE,
			MUIA_HTMLview_Scrollbars, MUIV_HTMLview_Scrollbars_Auto,
			End,

		End;

	get(htmlview, MUIA_ScrollGroup_HTMLview, &htmlview);
	CreateSampleText(htmlview);

	SetAttrs(snoop,
		MUIA_Weight, 0,
		MUIA_CycleChain, TRUE,
		MUIA_InputMode, MUIV_InputMode_Toggle,
		TAG_DONE);

	DoMethod(snoop, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, objs[PageScrollKey], 3, MUIM_Set, MUIA_HotkeyString_Snoop, MUIV_TriggerValue);
	DoMethod(snoop, MUIM_Notify, MUIA_Selected, TRUE, MUIV_Notify_Window, 3, MUIM_Set, MUIA_Window_ActiveObject, PageScrollKey);

	DoMethod(htmlview, MUIM_Notify, MUIA_HTMLview_ClickedURL, MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_HTMLview_GotoURL, MUIV_TriggerValue, NULL);
	DoMethod(htmlview, MUIM_Notify, MUIA_AppMessage,          MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, &AppMessageHook, MUIV_TriggerValue);

	return group;
}
