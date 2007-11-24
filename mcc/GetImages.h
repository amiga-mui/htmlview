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

#ifndef GETIMAGES_H
#define GETIMAGES_H

#include <string.h>
#include <proto/datatypes.h>
#include <new>

struct ObjectList
{
  ObjectList (class SuperClass *obj)
  {
    Obj = obj;
  }

  ~ObjectList ()
  {
    delete Next;
  }

  struct ObjectList *Next;
  class SuperClass *Obj;
};

struct ImageList
{
  ImageList (STRPTR imagename, ULONG width, ULONG height, class SuperClass *obj)
  {
    ImageName = imagename;
    Width = width;
    Height = height;
    Objects = new (std::nothrow) struct ObjectList(obj);
    ObjectsLast = Objects;
  }

  ~ImageList ()
  {
    delete Next;
    delete Objects;
    delete ImageName;
  }

  VOID AddImage (STRPTR imagename, ULONG width, ULONG height, class SuperClass *obj)
  {
    if(!strcmp(imagename, ImageName) && Width == width && Height == height)
    {
      ObjectsLast->Next = new (std::nothrow) struct ObjectList(obj);
      ObjectsLast = ObjectsLast->Next;

      delete imagename;
    }
    else
    {
      if(Next)
          Next->AddImage(imagename, width, height, obj);
      else  Next = new (std::nothrow) struct ImageList(imagename, width, height, obj);
    }
  }

  struct ImageList *Next;
  STRPTR ImageName;
  ULONG Width, Height;
  struct ObjectList *Objects;
  struct ObjectList *ObjectsLast;
};


struct GetImagesMessage
{
  GetImagesMessage (Object *htmlview)
  {
    HTMLview = htmlview;
    Images = NULL;
  }

  VOID AddImage (STRPTR imagename, ULONG width, ULONG height, class SuperClass *obj)
  {
    if(Images)
        Images->AddImage(imagename, width, height, obj);
    else  Images = new (std::nothrow) struct ImageList(imagename, width, height, obj);
  }

  Object *HTMLview;
  struct ImageList *Images;
};

#endif
