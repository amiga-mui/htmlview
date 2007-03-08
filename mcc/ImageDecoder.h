#ifndef IMAGEDECODER_H
#define IMAGEDECODER_H

#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif

#ifndef COLOURMANAGER_H
struct RGBPixel
{
	UBYTE R, G, B, A;
};
#endif

#define IDV_Version 2

enum
{
	IDA_Version = TAG_USER,

	IDA_Screen,				/* struct Screen * */
	IDA_NoDither,			/* BOOL */
	IDA_Width,				/* ULONG */
	IDA_Height,				/* ULONG */
	IDA_File,				/* STRPTR - might be an URL :-) */
	IDA_Gamma,				/* ULONG */

	IDM_Decode,
	IDM_Read,
	IDM_Skip,
	IDM_SetDimensions,
	IDM_GetLineBuffer,
	IDM_DrawLineBuffer,

	IDA_BGColour,			/* 0x00RRGGBB */

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

struct IDP_AllocateFrameTags
{
	ULONG MethodID;
	ULONG Width, Height;
	struct TagItem Tags;
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
