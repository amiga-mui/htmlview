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

#ifndef IMAGEDECODER_H
#define IMAGEDECODER_H

#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif

#if defined(__PPC__)
  #if defined(__GNUC__)
    #pragma pack(2)
  #elif defined(__VBCC__)
    #pragma amiga-align
  #endif
#endif

#ifndef COLOURMANAGER_H
struct RGBPixel
{
  UBYTE R, G, B, A;
};
#endif

struct IDP_SetDimensions
{
  ULONG MethodID;
  ULONG Width, Height;
  LONG Interlaced; /* NONE, NORMAL or ADAM7 */

  /* Animation */
  ULONG LeftOfs, TopOfs;
  ULONG AnimDelay, Disposal;
  struct RGBPixel Background;
};

struct IDP_AllocateFrameTags
{
  ULONG MethodID;
  ULONG Width, Height;
  struct TagItem Tags;
};

#if defined(__PPC__)
  #if defined(__GNUC__)
    #pragma pack()
  #elif defined(__VBCC__)
    #pragma default-align
  #endif
#endif

#define IDV_Version 2

enum
{
  IDA_Version = TAG_USER,

  IDA_Screen,       /* struct Screen * */
  IDA_NoDither,     /* BOOL */
  IDA_Width,        /* ULONG */
  IDA_Height,       /* ULONG */
  IDA_File,       /* STRPTR - might be an URL :-) */
  IDA_Gamma,        /* ULONG */

  IDM_Decode,
  IDM_Read,
  IDM_Skip,
  IDM_SetDimensions,
  IDM_GetLineBuffer,
  IDM_DrawLineBuffer,

  IDA_BGColour,     /* 0x00RRGGBB */

  IDM_AllocateFrameTags,

  IDA_ErrorString,
  IDA_WarningString,
  IDA_StringParms,

  IDM_ExhaustiveRead,

  IDV_NumberOf
};

struct IDP_ExhaustiveRead
{
  ULONG MethodID;
  UBYTE *Buffer;
  ULONG MaxLength;
};

struct IDP_Read
{
  ULONG MethodID;
  UBYTE *Buffer;
  ULONG Length;
};

struct IDP_Skip
{
  ULONG MethodID;
  ULONG Length;
};

enum
{
  InterlaceNONE = 0,
  InterlaceNORMAL,
  InterlaceADAM7
};

enum
{
  DisposeUNKNOWN = 0,
  DisposeNOP,
  DisposeBACKGROUND,
  DisposePREVIOUS
};

struct IDP_GetLineBuffer
{
  ULONG MethodID;
  ULONG Y;
  LONG LastPass; /* BOOL */
};

struct IDP_DrawLineBuffer
{
  ULONG MethodID;
  struct RGBPixel *LineBuffer;
  ULONG Y;
  ULONG Height;
};

enum
{
  TransparencyNONE,
  TransparencySINGLE,
  TransparencyALPHA
};

enum
{
  AFA_Interlaced = TAG_USER,
  AFA_LeftOfs,
  AFA_TopOfs,
  AFA_AnimDelay,
  AFA_Disposal,
  AFA_Background,
  AFA_Transparency,
  AFA_LoopCount,

  AFA_NumberOf
};


#endif
