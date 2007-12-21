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
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/dos.h>
#include <libraries/mui.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>

#include "muiextra.h"
#include "HTMLview_mcc.h"
#include "private.h"
#include "rev.h"
#include "ScrollGroup.h"
#include "locale.h"

/*#include <proto/exec.h>
#define NewRawDoFmt(__p0, __p1, __p2, ...) \
    (((STRPTR (*)(void *, CONST_STRPTR , APTR (*)(APTR, UBYTE), STRPTR , ...))*(void**)((long)(EXEC_BASE_NAME) - 922))((void*)(EXEC_BASE_NAME), __p0, __p1, __p2, __VA_ARGS__))*/

struct PrefsExchangeData { ULONG ObjIndex, Tag, CfgItem; LONG Length; const char *DefValue; const APTR ItemName; };

static struct PrefsExchangeData PrefsInfo[] =
{
    { SmallFont,              MUIA_String_Contents,   MUICFG_HTMLview_SmallFont,          0, "XHelvetica/9",                 MSG_Fonts_Small},
    { NormalFont,             MUIA_String_Contents,   MUICFG_HTMLview_NormalFont,         0, "XHelvetica/11",                MSG_Fonts_Normal},
    { FixedFont,              MUIA_String_Contents,   MUICFG_HTMLview_FixedFont,          0, "XEN/11",                       MSG_Fonts_Normal},
    { LargeFont,              MUIA_String_Contents,   MUICFG_HTMLview_LargeFont,          0, "XHelvetica/13",                MSG_Fonts_Large},
    { H1,                     MUIA_String_Contents,   MUICFG_HTMLview_H1,                 0, "Helvetica/24",                 MSG_Fonts_H1},
    { H2,                     MUIA_String_Contents,   MUICFG_HTMLview_H2,                 0, "Helvetica/18",                 MSG_Fonts_H2},
    { H3,                     MUIA_String_Contents,   MUICFG_HTMLview_H3,                 0, "Helvetica/15",                 MSG_Fonts_H3},
    { H4,                     MUIA_String_Contents,   MUICFG_HTMLview_H4,                 0, "XHelvetica/13",                MSG_Fonts_H4},
    { H5,                     MUIA_String_Contents,   MUICFG_HTMLview_H5,                 0, "XHelvetica/11",                MSG_Fonts_H5},
    { H6,                     MUIA_String_Contents,   MUICFG_HTMLview_H6,                 0, "XHelvetica/9",                 MSG_Fonts_H6},
    { IgnoreDocCols,          MUIA_Selected,          MUICFG_HTMLview_IgnoreDocCols,      -4, (STRPTR)FALSE,                 MSG_Colours_IgnoreColours},
    { Col_Background,         MUIA_Pendisplay_Spec,   MUICFG_HTMLview_Col_Background,     32, "m1",                          MSG_Colours_Backgound},
    { Col_Text,               MUIA_Pendisplay_Spec,   MUICFG_HTMLview_Col_Text,           32, "m5",                          MSG_Colours_Text},
    { Col_Link,               MUIA_Pendisplay_Spec,   MUICFG_HTMLview_Col_Link,           32, "r00000000,00000000,99",       MSG_Colours_NormalLink},
    { Col_VLink,              MUIA_Pendisplay_Spec,   MUICFG_HTMLview_Col_VLink,          32, "rAAAAAAAA,00000000,00",       MSG_Colours_VisitedLink},
    { Col_ALink,              MUIA_Pendisplay_Spec,   MUICFG_HTMLview_Col_ALink,          32, "rDCDCDCDC,DCDCDCDC,00",       MSG_Colours_SelectedLink},
    { ListItemFile,           MUIA_String_Contents,   MUICFG_HTMLview_ListItemFile,       0,  "PROGDIR:Images/ListItems",    MSG_Images_Listitems},
    { DitherType,             MUIA_Cycle_Active,      MUICFG_HTMLview_DitherType,         -4, (STRPTR)0,                     MSG_Images_Dither},
    { ImageCacheSize,         MUIA_Numeric_Value,     MUICFG_HTMLview_ImageCacheSize,     -4, (STRPTR)51,                    MSG_Images_CacheSize},
    { GammaCorrection,        MUIA_Numeric_Value,     MUICFG_HTMLview_GammaCorrection,    -4, (STRPTR)22,                    MSG_Images_Gamma},
    { PageScrollSmooth,       MUIA_Selected,          MUICFG_HTMLview_PageScrollSmooth,   -4, (STRPTR)TRUE,                  MSG_PageScroll_Smooth},
    { PageScrollKey,          MUIA_String_Contents,   MUICFG_HTMLview_PageScrollKey,       0, "-repeat space",               MSG_PageScroll_Key},
    { PageScrollMove,         MUIA_Numeric_Value,     MUICFG_HTMLview_PageScrollMove,     -4, (STRPTR)70,                    MSG_PageScroll_Move},
    { -1, 0, 0, 0, NULL , NULL}
};

struct MUIP_AppMessage { struct AppMessage *appmsg; };

HOOKPROTONH(AppMessageCode, VOID, Object* htmlview, struct MUIP_AppMessage *args)
{
    if(args->appmsg->am_NumArgs)
    {
      char dir[256+8];

      #if defined(__MORPHOS__)
      stccpy(dir, "file://", sizeof(dir));
      #else
      strlcpy(dir, "file://", sizeof(dir));
      #endif

        if(NameFromLock(args->appmsg->am_ArgList[0].wa_Lock, dir+7, 256))
        {
            AddPart(dir+7, args->appmsg->am_ArgList[0].wa_Name, 256);
            DoMethod(htmlview, MUIM_HTMLview_GotoURL, (ULONG)dir, NULL);
        }
    }
}
MakeStaticHook(AppMessageHook, AppMessageCode);

VOID CreateSampleText(Object *htmlview)
{
    ULONG ver, rev;
    char text[800];

    ver = xget(htmlview, MUIA_Version);
    rev = xget(htmlview, MUIA_Revision);

    snprintf(text, sizeof(text),GetStr(MSG_Page_SampleText),ver, rev, LIB_VERSION, LIB_REVISION);
    set(htmlview, MUIA_HTMLview_Contents, (ULONG)text);
}

DISPATCHER(_DispatcherP)
{
    struct InstData_MCP *data = (struct InstData_MCP *)INST_DATA(cl, obj);
    ULONG result = 0;

    if (msg->MethodID!=OM_NEW)
        data = (struct InstData_MCP *)INST_DATA(cl, obj);

    switch(msg->MethodID)
    {
        case OM_NEW:
        {
            if((obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg)))
            {
                Object *group;

                data = (struct InstData_MCP *)INST_DATA(cl, obj);

                if((group = CreatePrefsGroup(obj,data)))
                {
                    ULONG i;

                    DoMethod(obj, OM_ADDMEMBER, (ULONG)group);

                    for(i = 0; i < NumberOfObjects; i++)
                        DoMethod(obj, MUIM_Mccprefs_RegisterGadget, (ULONG)data->Objects[PrefsInfo[i].ObjIndex], (ULONG)PrefsInfo[i].CfgItem, 1L, (ULONG)GetStr(PrefsInfo[i].ItemName));

                    result = (ULONG)obj;
                }
                else
                {
                    CoerceMethod(cl, obj, OM_DISPOSE);
                }
            }
        }
        break;

        case MUIM_Settingsgroup_ConfigToGadgets:
        {
            Object *configdata = ((struct MUIP_Settingsgroup_ConfigToGadgets *)msg)->configdata;

            struct PrefsExchangeData *item = PrefsInfo;
            while((LONG)item->ObjIndex != -1)
            {
                STRPTR cfg_val;
                if((cfg_val = (STRPTR)DoMethod(configdata, MUIM_Dataspace_Find, item->CfgItem)))
                {
                    if(item->Length < 0)
                        cfg_val = *(STRPTR *)cfg_val;
                }
                else
	            cfg_val = (STRPTR)item->DefValue;

                set(data->Objects[item->ObjIndex], item->Tag, (ULONG)cfg_val);
                item++;
            }
        }
        break;

        case MUIM_Settingsgroup_GadgetsToConfig:
        {
            Object *configdata = ((struct MUIP_Settingsgroup_ConfigToGadgets *)msg)->configdata;

            struct PrefsExchangeData *item = PrefsInfo;
            while((LONG)item->ObjIndex != -1)
            {
                STRPTR cfg_val;
                LONG len;

                cfg_val = (STRPTR)xget(data->Objects[item->ObjIndex], item->Tag);

                if((len = item->Length) < 0)
                  DoMethod(configdata, MUIM_Dataspace_Add, (ULONG)&cfg_val, 4, (ULONG)item->CfgItem);
                else
                    DoMethod(configdata, MUIM_Dataspace_Add, (ULONG)cfg_val, len ? len : (LONG)strlen(cfg_val)+1, (ULONG)item->CfgItem);

                item++;
            }
        }
        break;

        case MM_HTMLview_CreateSample:
        {
            if (!data->Sample)
            {
                Object *sg;

				sg = HTMLviewObject,
					VirtualFrame,
					MUIA_CycleChain, TRUE,
					MUIA_HTMLview_Scrollbars, MUIV_HTMLview_Scrollbars_Auto,
				End;

                if (sg)
                {
                    Object *hview;

					hview = (Object *)xget(sg,MUIA_ScrollGroup_HTMLview);
                    if (!hview) return 0;

                    CreateSampleText(hview);
                    DoMethod(hview,MUIM_Notify,MUIA_HTMLview_ClickedURL,MUIV_EveryTime,MUIV_Notify_Self,3,MUIM_HTMLview_GotoURL,MUIV_TriggerValue, NULL);
                    DoMethod(hview,MUIM_Notify,MUIA_AppMessage,MUIV_EveryTime,MUIV_Notify_Self,3,MUIM_CallHook,(ULONG)&AppMessageHook,MUIV_TriggerValue);

                    DoMethod(data->SampleGroup,MUIM_Group_InitChange);
                    DoMethod(data->SampleGroup,OM_REMMEMBER,(ULONG)data->FirstSample);
                    DoMethod(data->SampleGroup,OM_ADDMEMBER,(ULONG)sg);
                    DoMethod(data->SampleGroup,MUIM_Group_ExitChange);

					MUI_DisposeObject(data->FirstSample);
                    data->Sample = sg;
                }
            }
        }
        break;

        default:
            result = DoSuperMethodA(cl, obj, msg);
        break;
    }
    return result;
}

struct CacheSliderData
{
    char Buffer[16];
};

DISPATCHER(CacheSliderDispatcher)
{
    ULONG result;
    if(msg->MethodID == MUIM_Numeric_Stringify)
    {
        struct CacheSliderData *data = (struct CacheSliderData *)INST_DATA(cl, obj);
        struct MUIP_Numeric_Stringify *smsg = (struct MUIP_Numeric_Stringify *)msg;
        char *buf = data->Buffer;

        ULONG val = smsg->value;
        if(val < 103)
          snprintf(buf, sizeof(data->Buffer), "%ld KB", val);
        else
        snprintf(buf, sizeof(data->Buffer), "%ld.%ld MB", val/1024, (10*(val%1024))/1024);

        result = (ULONG)buf;
    }
    else
        result = DoSuperMethodA(cl, obj, msg);

    return result;
}

struct GammaSliderData
{
    char Buffer[16];
};

DISPATCHER(GammaSliderDispatcher)
{
    ULONG result;
    if(msg->MethodID == MUIM_Numeric_Stringify)
    {
        struct GammaSliderData *data = (struct GammaSliderData *)INST_DATA(cl, obj);
        struct MUIP_Numeric_Stringify *smsg = (struct MUIP_Numeric_Stringify *)msg;
        char *buf = data->Buffer;

        ULONG val = smsg->value;
        snprintf(buf, sizeof(data->Buffer), "%ld.%03ld", val/1000, val%1000);

        result = (ULONG)buf;
    }
    else
    {
        result = DoSuperMethodA(cl, obj, msg);
    }
    return result;
}

struct MUI_CustomClass *CacheSliderClass = NULL;
struct MUI_CustomClass *GammaSliderClass = NULL;

#if defined(__MORPHOS__)
extern struct MUI_CustomClass *PicClass;
#endif

BOOL CreateSubClasses(void)
{
  if((CacheSliderClass = MUI_CreateCustomClass(NULL, MUIC_Slider, NULL, sizeof(struct CacheSliderData), ENTRY(CacheSliderDispatcher))))
  {
    if((GammaSliderClass = MUI_CreateCustomClass(NULL, MUIC_Slider, NULL, sizeof(struct GammaSliderData), ENTRY(GammaSliderDispatcher))))
    {
      #if defined(__MORPHOS__)
      if (initPicClass())
      {
        return TRUE;
      }
      #else
      return TRUE;
      #endif
    }
  }

  return FALSE;
}

void DeleteSubClasses(void)
{
  if(GammaSliderClass)
  {
    MUI_DeleteCustomClass(GammaSliderClass);
    GammaSliderClass = NULL;
  }

  if(CacheSliderClass)
  {
    MUI_DeleteCustomClass(CacheSliderClass);
    CacheSliderClass = NULL;
  }

#if defined(__MORPHOS__)
  if(PicClass)
  {
    MUI_DeleteCustomClass(PicClass);
    PicClass = NULL;
  }
#endif
}
