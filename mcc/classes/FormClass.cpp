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

#include "FormClass.h"

#include "Forms.h"
#include "ParseMessage.h"
//#include "private.h"
#include "ScanArgs.h"
#include "SharedData.h"
#include <stdio.h>
#include <new>

VOID FormClass::AppendGadget (struct AppendGadgetMessage &amsg)
{
  class FormClass *oldform = amsg.Form;
  amsg.Form = this;
  TreeClass::AppendGadget(amsg);
  amsg.Form = oldform;
}

VOID FormClass::ExportForm (struct ExportFormMessage &emsg)
{
  if(&emsg == NULL)
  {
    struct HTMLviewData *data;

    data = (struct HTMLviewData *)xget(HTMLview, MUIA_HTMLview_InstanceData);

    if(Method == Form_Get && Action)
    {
      struct ExportFormMessage emsg2;
      TreeClass::ExportForm(emsg2);

      STRPTR base = (STRPTR)DoMethod(HTMLview, MUIM_HTMLview_AddPart, (ULONG)Action);
      ULONG len = strlen(base) + emsg2.StrLength + 1;
      STRPTR url = new (std::nothrow) char[len];
      if (!url) return;
      ULONG index = sprintf(url, "%s?", base ? base : "");
      if (base) delete base;

      emsg2.GetElements(url+index);

      SetAttrs(data->Share->Obj,
          MUIA_HTMLview_ClickedURL, (ULONG)url,
          MUIA_HTMLview_Target,     (ULONG)FindTarget(HTMLview, Target, data),
          MUIA_HTMLview_Qualifier,  0L,
          TAG_DONE);

      delete url;
    }
    else if(Method == Form_Post && Action)
    {
      struct ExportFormMessage emsg2;
      TreeClass::ExportForm(emsg2);

      STRPTR base = (STRPTR)DoMethod(HTMLview, MUIM_HTMLview_AddPart, (ULONG)Action);
      STRPTR contents = new (std::nothrow) char[emsg2.StrLength + 1];
      if (!contents) return;
      emsg2.GetElements(contents);

      DoMethod(HTMLview, MUIM_HTMLview_Post, (ULONG)base, (ULONG)Target, (ULONG)contents, (ULONG)(EncType ? EncType : "application/x-www-form-urlencoded"));
      delete base;
//      This is deleted by the ParseThread
//      delete contents;
    }
  }
  else
  {
    TreeClass::ExportForm(emsg);
  }
}

VOID FormClass::ResetForm ()
{
  TreeClass::ResetForm();
}

VOID FormClass::Parse(REG(a2, struct ParseMessage &pmsg))
{
  pmsg.SetLock();
  pmsg.NextEndBracket();
#ifdef OUTPUT
  PrintTag(pmsg.Locked);
#endif

  const char *MethodKeywords[] = { "GET", "POST", NULL };
  struct ArgList args[] =
  {
    { "METHOD",   &Method,  ARG_KEYWORD, MethodKeywords },
    { "ACTION",   &Action,  ARG_URL,     NULL           },
    { "TARGET",   &Target,  ARG_URL,     NULL           },
    { "ENCTYPE",  &EncType, ARG_STRING,  NULL           },
    { NULL,       NULL,     0,           NULL           }
  };
  ScanArgs(pmsg.Locked, args);

  HTMLview = pmsg.HTMLview;

  UBYTE p = pmsg.OpenCounts[tag_P];
  pmsg.OpenCounts[tag_P] = 0;
  TreeClass::Parse(pmsg);
  pmsg.OpenCounts[tag_P] = p;
}

