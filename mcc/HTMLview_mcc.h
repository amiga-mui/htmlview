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

#ifndef MCC_HTMLVIEW
#define MCC_HTMLVIEW

#include <exec/types.h>

#define MUIC_HTMLview   "HTMLview.mcc"
#define HTMLviewObject  MUI_NewObject(MUIC_HTMLview

#define HTMLview_ID(x) (0xad003000 + x)

/** \mainpage HTMLview Documentation
  * \section clicked_sec MUI Attributes and Methods
  * Please see htmlview-mcc/trunk/mcc/HTMLview_mcc.h for 
  * documented attributes and methods
  */

/** Private 
  */
#define MUIM_HTMLview_ExtMessage          HTMLview_ID(1)		/* Private */

/**
  * Go to the specified URL, and use a named frame as target.
  * 
  * NAME
  *      
  * MUIM_HTMLview_GotoURL
  *
  * SYNOPSIS
  *
  * DoMethod(obj, MUIM_HTMLview_GotoURL, STRPTR url, STRPTR target);
  *
  * FUNCTION
  * 
  * Go to the specified URL, and use a named frame as target. Give NULL as 
  * target address for the current frame. In contrary to MUIA_HTMLview_Contents, 
  * then this method will instantly return, because it starts a new task that 
  * will load and parse the page.
  *
  * RESULT
  *
  * A unique ID that has been asigned to the page. This ID will be given to the 
  * loadhook.
  * 
  * NOTES
  *
  * This method is likely to be replaced with a more flexible varargs version...
  *
  * SEE ALSO
  *
  * MUIA_HTMLview_Contents, MUIA_HTMLview_LoadHook
  */
#define MUIM_HTMLview_GotoURL             HTMLview_ID(2)

/** Private 
  */
#define MUIM_HTMLview_Period              HTMLview_ID(3)		/* Private */

/**
  * The title of the page. 
  *
  * NAME
  * 
  * MUIA_HTMLview_Title -- [..G], STRPTR
  *
  * FUNCTION
  * 
  * The title of the page. Reflects the contents of the < TITLE > tag
  */
#define MUIA_HTMLview_Title               HTMLview_ID(4)

/** 
  * Use this tag to set the contents to whatever you want displayed.
  *
  * NAME
  *
  * MUIA_HTMLview_Contents -- [IS.], STRPTR
  *
  * FUNCTION
  *
  * Use this tag to set the contents to whatever you want displayed. You 
  * currently need to embed the stuff that should be displayied in a Body-element. 
  * This may change, since the Body-tag is infact not required according to the 
  * HTML 4.0 specifications, but I really dislike this...
  *
  * EXAMPLE
  * 
  * SetAttrs(htmlview, MUIA_HTMLview_Contents, " Test example ", TAG_DONE);
  *
  * NOTE
  *
  * When you set the contents using this tag, then the page ID will change.
  * You can get the new page ID with MUIA_HTMLview_PageID
  *
  * SEE ALSO
  * 
  * MUIA_HTMLview_PageID
  */
#define MUIA_HTMLview_Contents            HTMLview_ID(5)

/**
  * This tag reflects which link is currently below the mousepointer.
  *
  * NAME
  * 
  * MUIA_HTMLview_CurrentURL -- [..G], STRPTR
  * 
  * FUNCTION
  * 
  * This tag reflects which link is currently below the mousepointer.
  * This tag is very similar to MUIA_HTMLview_ClickedURL.
  *
  * EXAMPLE
  *
  * // Setup a notify that will show the link in a text-object 
  *
  * DoMethod(htmlview, MUIM_Notify, MUIA_HTMLview_CurrentURL, MUIV_Everytime,
  * textbox, 3, MUIM_Set, MUIA_Text_Contents, MUIV_TriggerValue);
  * 
  * SEE ALSO
  *
  * MUIA_HTMLview_ClickedURL, MUIA_HTMLview_Target
  */
#define MUIA_HTMLview_CurrentURL          HTMLview_ID(7)

/** Private 
  */
#define MUIM_HTMLview_LoadImages          HTMLview_ID(8)    /* Private */

/** Private 
  */
#define MUIM_HTMLview_AddPart             HTMLview_ID(10)	/* Private */

/** Private 
  */
#define MUIM_HTMLview_DrawImages          HTMLview_ID(11)	/* Private */

/**
  * Setup a hook used for (page)-loading.
  *
  * NAME
  *
  * MUIA_HTMLview_LoadHook -- [I..], struct Hook *
  *
  * FUNCTION
  * 
  * Setup a hook used for (page)-loading. The hook is called with a pointer 
  * to itself in a0, a pointer to a struct HTMLview_LoadMsg in a1 and a 
  * pointer to the calling object in a2.
  *
  * This hook will be called from a separate task, so the only MUI method 
  * that you can use is MUIM_Application_PushMethod. The hook may very well 
  * be called by sevaral tasks at the same time, so your code needs to be 
  * re-entrant and thread-safe.
  *
  * The first field of the HTMLview_LoadMsg is lm_Type and can be either:
  *
  * HTMLview_Open
  *
  * You should open the file and return non-null for succes. The URL to open 
  * can be found as lmsg->lm_Open.URL. Please have a look in HTMLview_mcc.h 
  * for flag definitions. You can store a filehandle or similar in the 
  * lm_Userdata field.
  *
  * HTMLview_Read
  *
  * Read upto lmsg->lm_Read.Size bytes and store them in lmsg->lm_Read.Buffer. 
  * Return the actual number of bytes read. 0 for EOF.
  *
  * HTMLview_Write
  *
  * This is used when the user submit POST-forms. You can read the encoding 
  * type and length as EncodingType & PostLength.
  * 
  * HTMLview_Close
  *
  * Close the file.
  *
  * There is also a lm_PageID field, this is a unique identifier for the page 
  * requested. This ID is the same as returned by MUIM_HTMLview_GotoURL.
  * Please have in mind, when using frames, that two different frames may
  * use the same ID, but you can distingish by looking at the calling
  * object, which will be two different instances of HTMLview.mcc.
  * 
  * NOTES
  *
  * The object which originally requested something loaded, may not be valid, 
  * by the time you've actually loaded it (frames). So you should actually 
  * never use the obj pointer. Your LoadMsg contain a lm_App, which will 
  * always be valid.
  */
#define MUIA_HTMLview_LoadHook            HTMLview_ID(12)

/**
  * This will abort parsing of the current page.
  * 
  * NAME
  * 
  * MUIM_HTMLview_Abort
  *
  * SYNOPSIS
  *
  * DoMethod(obj, MUIM_HTMLview_Abort);
  *
  * FUNCTION
  *
  * This will abort parsing of the current page.
  */
#define MUIM_HTMLview_Abort               HTMLview_ID(13)

/**
  * Each anchor element will call this method to know whether the link they
  * hold has been visited before.
  * 
  * NAME
  *
  * MUIM_HTMLview_VLink
  * 
  * SYNOPSIS
  *
  * DoMethod(obj, MUIM_HTMLview_VLink, STRPTR url);
  *
  * FUNCTION
  *
  * Each anchor element will call this method to know whether the link they
  * hold has been visited before. This affects the rendering of whatever
  * elements the anchor contain.
  *
  * RESULT
  *
  * BOOL - Superclass returns FALSE.
  */
#define MUIM_HTMLview_VLink               HTMLview_ID(14)

/**
  * When the user follows a link, then this tag will reflect the complete URL 
  * of the pressed link, even if the HRef argument is only relative.
  *
  * NAME
  *
  * MUIA_HTMLview_ClickedURL -- [..G], STRPTR
  * 
  * FUNCTION
  *
  * When the user follows a link, then this tag will reflect the complete URL of 
  * the pressed link, even if the HRef argument is only relative. If the 
  * Anchor-element holds target info, then you can read this via the 
  * MUIA_HTMLview_Target tag. Note: The URL is only valid during the notify.
  *
  * EXAMPLE
  *      
  *	// Setup a notify that will just load the link that the user press 
  *      
  * DoMethod(htmlview, MUIM_Notify, MUIA_HTMLview_ClickedURL, MUIV_Everytime,
  *          MUIV_Notify_Self, 3, MUIM_HTMLview_GotoURL, MUIV_TriggerValue, NULL);
  *
  * SEE ALSO
  *
  * MUIA_HTMLview_Target, MUIA_HTMLview_Qualifier
  */
#define MUIA_HTMLview_ClickedURL          HTMLview_ID(15)

/**
  * When a separate task is parsing a page, then this method will be called 
  * approximately 5 times per second, with the number of parsed bytes as first 
  * argument.
  * 
  * NAME
  *
  * MUIM_HTMLview_Parsed
  *
  * SYNOPSIS
  *  
  * DoMethod(obj, MUIM_HTMLview_Parsed, ULONG parsed);
  *
  * FUNCTION
  *
  * When a separate task is parsing a page, then this method will be called 
  * approximately 5 times pr. second, with the number of parsed bytes as first 
  * argument. This is useful if you want to have a gauge object reflect the amount
  * of parsed data and perhaps also a KB/s count.
  */
#define MUIM_HTMLview_Parsed              HTMLview_ID(16)

/** Private 
  */
#define MUIM_HTMLview_PrivateGotoURL      HTMLview_ID(18)	/* Private */

/** Private 
  */
#define MUIM_HTMLview_AbortAll            HTMLview_ID(19)	/* Private */

/** Private 
  */
#define MUIM_HTMLview_LookupFrame         HTMLview_ID(20)	/* Private */

/**
  * When following an anchor, then the HTMLview-object will set this tag to 
  * the target-part of the anchor-element.
  *
  * NAME
  *
  * MUIA_HTMLview_Target -- [..G], STRPTR
  *
  * FUNCTION
  *
  * When following an anchor, then the HTMLview-object will set this tag
  * to the target-part of the anchor-element.
  *
  * SEE ALSO
  *
  * MUIA_HTMLview_ClickedURL, MUIA_HTMLview_Qualifier
  */
#define MUIA_HTMLview_Target              HTMLview_ID(21)

/** Private 
  */
#define MUIA_HTMLview_FrameName           HTMLview_ID(22)	/* Private */

/** Private 
  */
#define MUIA_HTMLview_MarginWidth         HTMLview_ID(23)	/* Private */

/** Private 
  */
#define MUIA_HTMLview_MarginHeight        HTMLview_ID(24)	/* Private */

/**
  * Reload the current page.
  *
  * NAME
  *
  * MUIM_HTMLview_Reload
  *
  * SYNOPSIS
  *
  * DoMethod(obj, MUIM_HTMLview_Reload);
  *
  * FUNCTION
  *
  * Reload the current page.
  */
#define MUIM_HTMLview_Reload              HTMLview_ID(25)

/** Private 
  */
#define MUIM_HTMLview_StartParser         HTMLview_ID(26)	/* Private */

/** Private 
  */
#define MUIM_HTMLview_HandleEvent         HTMLview_ID(27)	/* Private */

/** Private 
  */
#define MUIM_HTMLview_RemoveChildren      HTMLview_ID(28)	/* Private */

/** Currently Undocumented
  */
#define MUIA_HTMLview_IPC                 HTMLview_ID(29)

/**
  * Since several built-in MUI gadgets don't use eventhandlers, then this
  * class will only react on input, if it's either active, or default with
  * no other active objects.
  * 
  * NAME
  *
  * MUIA_HTMLview_DiscreteInput -- [IS.], BOOL
  *
  * FUNCTION
  *
  * Since several built-in MUI gadgets don't use eventhandlers, then this
  * class will only react on input, if it's either active, or default with
  * no other active objects.
  * 
  * If you set the tag to FALSE, then it will always try to use the events
  * that it receive. This is desireable if you for example have an active
  * stringgadget and press arrow up/down, since the stringgadget can't use
  * these keys, and therfor pass 'em on, however only if you use one of
  * the stringgadget replacements, which use an eventhandler for input.
  *
  * NOTE
  * 
  * The default value of this tag is currently TRUE, but it may change,
  * when MUI's built-in objects start to use eventhandlers. So either set
  * the tag to FALSE, or leave it as it is.
  */
#define MUIA_HTMLview_DiscreteInput       HTMLview_ID(30)

/**
  * Search for the given string.
  *
  * NAME
  *
  * MUIM_HTMLview_Search
  * 
  * SYNOPSIS
  * 
  * DoMethod(obj, MUIM_HTMLview_Search, STRPTR string, ULONG flags);
  *
  * FUNCTION
  *
  * Search for the given string.
  * 
  * Flags can be:
  * <pre>
  *      o MUIF_HTMLview_Search_CaseSensitive:
  *        The string must be an exact match.
  *      o MUIF_HTMLview_Search_DOSPattern:   (not yet implemented)
  *        The string is to be treated as a DOSPattern.
  *        The class will call ParsePattern(NoCase).
  *      o MUIF_HTMLview_Search_Backwards:    (not yet implemented)
  *        Search from the bottom and up.
  *      o MUIF_HTMLview_Search_FromTop:
  *        The search normally starts at the top of the page, if you set this
  *        flag, then it will start at the top of the document instead.
  *        This flag does not make sense together with
  *        MUIF_HTMLview_Search_Backwards nor MUIF_HTMLview_Search_Next.
  *      o MUIF_HTMLview_Search_Next:
  *        The search will start at the last found string.
  * </pre>
  * NOTES
  *
  * The search string mustn't exceed 120 characters.
  */
#define MUIM_HTMLview_Search              HTMLview_ID(31)

/** Private 
  */
#define MUIA_HTMLview_IntuiTicks          HTMLview_ID(32)	/* Private */

/**
  * This tag will give a reasonable delta factor for a horizontal scrollbar.
  *
  * NAME
  * 
  * MUIA_HTMLview_Prop_HDeltaFactor -- [..G], ULONG
  *
  * FUNCTION
  *
  * This tag will give a reasonable delta factor for a horizontal scrollbar.
  * 
  * SEE ALSO
  *
  * MUIA_Virtgroup_Width, MUIA_Virtgroup_Left, MUIA_Width
  */
#define MUIA_HTMLview_Prop_HDeltaFactor   HTMLview_ID(33)

/**
  * This tag will give a reasonable delta factor for a vertical scrollbar.
  *
  * NAME
  *
  * MUIA_HTMLview_Prop_VDeltaFactor -- [..G], ULONG
  *
  * FUNCTION
  *
  * This tag will give a reasonable delta factor for a vertical scrollbar.
  * 
  * SEE ALSO
  *
  * MUIA_Virtgroup_Height, MUIA_Virtgroup_Top, MUIA_Height
  */
#define MUIA_HTMLview_Prop_VDeltaFactor   HTMLview_ID(34)

/**
  * If you supply this tag, then you will get back a scrollgroup object,
  * which contain the htmlview object as child.
  *
  * NAME
  *
  * MUIA_HTMLview_Scrollbars -- [I..], ULONG
  *
  * SPECIAL INPUTS
  *
  * MUIV_HTMLview_Scrollbars_Auto
  *
  * MUIV_HTMLview_Scrollbars_HorizAuto
  * 
  * MUIV_HTMLview_Scrollbars_No
  *
  * MUIV_HTMLview_Scrollbars_Yes
  *
  * FUNCTION
  * 
  * If you supply this tag, then you will get back a scrollgroup object, which 
  * contain the htmlview object as child. You can't really use this scrollgroup 
  * object (other than having it as part of your application) so you'll need to 
  * obtain the pointer to the htmlview object. This can be done via 
  * MUIA_ScrollGroup_HTMLview.
  * 
  * EXAMPLE
  * 
  * get(scrollgroup, MUIA_ScrollGroup_HTMLview, &htmlview);
  *
  * If you don't want scrollbars, then you shouldn't use this tag. If you always 
  * want scrollbars, then you should instead encapsulate the htmlview object in 
  * a real scrollgroup, or setup your own notifications.
  * 
  * NOTE
  * 
  * If you use this tag with a subclass, then your OM_NEW needs a little bit of 
  * modification, here's how:
  * 
  * <pre>
  *      case OM_NEW:
  *      {
  *          if(result = DoSuperMethodA(cl, obj, msg))
  *          {
  *             get(result, MUIA_ScrollGroup_HTMLview, &obj);
  *             struct mydata *data = INST_DATA(cl, obj);
  *             ...
  *          }
  *          return(result);
  *      }
  *      break;
  * </pre>
  * EXAMPLE
  *
  * //Create an instance with scrollbars, which will automatically appear when needed 
  *
  * Object *dummy, *htmlview;
  *
  * ...
  *
  * Child, dummy = HTMLviewObject, VirtualFrame, MUIA_HTMLview_Contents, " Test ",
  * MUIA_HTMLview_Scrollbars, MUIV_HTMLview_Scrollbars_Auto, End,
  *
  * ...
  *      
  * //The scrollgroup has been created as part of the application tree.
  *
  * //Now we need a pointer to the *real* HTMLview object 
  *
  * get(dummy, MUIA_ScrollGroup_HTMLview, &htmlview);
  * 
  * //We now have a pointer, so we'll setup some notifies... 
  * 
  * DoMethod(htmlview, MUIM_Notify, MUIA_HTMLview_Title, MUIV_EveryTime, 
  * MUIV_Notify_Window, 3, MUIM_Set, MUIA_Window_Title, MUIV_TriggerValue);
  *
  * ...
  */
#define MUIA_HTMLview_Scrollbars          HTMLview_ID(35)

/** Private 
  */
#define MUIM_HTMLview_StartRefreshTimer   HTMLview_ID(36)	/* Private */

/** Private 
  */
#define MUIM_HTMLview_Refresh             HTMLview_ID(38)	/* Private */

/**
  * Returns info about the context of the (x, y) coordinate.
  *
  * NAME
  *
  * MUIM_HTMLview_GetContextInfo
  *
  * SYNOPSIS
  *
  * DoMethod(obj, MUIM_HTMLview_GetContextInfo, LONG X, LONG Y);
  *
  * FUNCTION
  *
  * Returns info about the context of the (x, y) coordinate.
  * The coordinates are relative to the upper left corner of the window.
  *
  * RESULT
  * 
  * A pointer to a MUIR_HTMLview_GetContextInfo structure or NULL of there
  * is no context.
  *
  * The structure currently holds these fields:
  *
  * URL - Complete URL of the link found at the given position or NULL if no 
  * link was found.
  *
  * Target - Target value of the potential link, at the given position.
  *
  * Img - Complete URL of the image found at the given position or NULL if no 
  * image was found.
  * 
  * Frame - The URL of the frame, which the given position belongs to or NULL 
  * if no frame exist for the given coordinate.
  *
  * Background - URL of the background image, if any. The structure will grow, 
  * please have a look in HTMLview_mcc.h for more fields...
  */
#define MUIM_HTMLview_GetContextInfo      HTMLview_ID(39)

/** Private 
  */
#define MUIM_HTMLview_HitTest             HTMLview_ID(40)	/* Private */

/**
  * The URL of the page. 
  *
  * NAME
  *
  * MUIA_HTMLview_URL -- [..G], STRPTR
  * 
  * FUNCTION
  *
  * The URL of the page. This is just a copy of what you gave to 
  * MUIM_HTMLview_GotoURL.
  */
#define MUIA_HTMLview_URL                 HTMLview_ID(41)

/**
  * The qualifier(s) which the user pressed, when she clicked on a link,
  * is registered and can be obtained with this tag.
  *
  * NAME
  *
  * MUIA_HTMLview_Qualifier -- [..G], ULONG
  *
  * FUNCTION
  *
  * The qualifier(s) which the user pressed, when she clicked on a link,
  * is registered and can be obtained with this tag.
  * 
  * SEE ALSO
  *
  * MUIA_HTMLview_ClickedURL
  */
#define MUIA_HTMLview_Qualifier           HTMLview_ID(42)

/**
  * Setup a hook used for image-loading.
  * 
  * NAME
  *
  * MUIA_HTMLview_ImageLoadHook -- [I..], struct Hook *
  *
  * FUNCTION
  *
  * Setup a hook used for image-loading.
  *
  * SEE ALSO
  *
  * MUIA_HTMLview_LoadHook
  */
#define MUIA_HTMLview_ImageLoadHook       HTMLview_ID(44)

/** Private 
  */
#define MUIM_HTMLview_AnimTick            HTMLview_ID(45)	/* Private */

/** Private 
  */
#define MUIM_HTMLview_AddSingleAnim       HTMLview_ID(48)	/* Private */

/**
  * Get the number of pictures which are currently being loaded/decoded.
  *
  * NAME
  *
  * MUIA_HTMLview_ImagesInDecodeQueue -- [..G], ULONG
  *
  * FUNCTION
  *
  * Get the number of pictures which are currently being loaded/decoded.
  */
#define MUIA_HTMLview_ImagesInDecodeQueue HTMLview_ID(49)

/**
  * Remove the given URL from the image-cache.
  *
  * NAME
  *
  * MUIM_HTMLview_FlushImage
  *
  * SYNOPSIS
  *
  * DoMethod(obj, MUIM_HTMLview_FlushImage, STRPTR url);
  * 
  * FUNCTION
  *
  * Remove the given URL from the image-cache.
  * To flush several images, then URL can be one of
  *
  * MUIV_HTMLview_FlushImage_All
  *
  * MUIV_HTMLview_FlushImage_Displayed
  *
  * MUIV_HTMLview_FlushImage_Nondisplayed
  */
#define MUIM_HTMLview_FlushImage          HTMLview_ID(50)

/** Private 
  */
#define MUIA_HTMLview_SharedData          HTMLview_ID(51)	/* Private */

/** Private 
  */
#define MUIM_HTMLview_ServerRequest       HTMLview_ID(52)	/* Private */

/**
  * Get the page ID of the currently displayed page.
  * 
  * NAME
  *
  * MUIA_HTMLview_PageID -- [..G], ULONG
  * 
  * FUNCTION
  *
  * Get the page ID of the currently displayed page. This is the same as 
  * returned by MUIM_HTMLview_GotoURL.
  *
  * SEE ALSO
  *
  * MUIA_HTMLview_Contents
  */
#define MUIA_HTMLview_PageID              HTMLview_ID(53)

/**
  * This will pause all animations.
  *
  * NAME
  *
  * MUIM_HTMLview_PauseAnims
  *
  * SYNOPSIS
  *
  * DoMethod(obj, MUIM_HTMLview_PauseAnims);
  *
  * FUNCTION
  *
  * This will pause all animations.
  * 
  * NOTE
  *
  * This method is automatically invoked when the window goes inactive.
  * 
  * SEE ALSO
  * 
  * MUIM_HTMLview_ContinueAnims
  */
#define MUIM_HTMLview_PauseAnims          HTMLview_ID(54)	

/**
  * This will continue animations which were previously paused with 
  * MUIM_HTMLview_PauseAnims.
  * 
  * NAME
  *
  * MUIM_HTMLview_ContinueAnims
  *
  * SYNOPSIS
  *
  * DoMethod(obj, MUIM_HTMLview_ContinueAnims);
  *
  * FUNCTION
  *
  * This will continue animations which were previously paused with 
  * MUIM_HTMLview_PauseAnims.
  *
  * NOTE
  *
  * This method is automatically invoked when the window goes active.
  *
  * SEE ALSO
  *
  * MUIM_HTMLview_PauseAnims
  */
#define MUIM_HTMLview_ContinueAnims       HTMLview_ID(55)

/** Currently Undocumented
  */
#define MUIM_HTMLview_Post                HTMLview_ID(56)	

/** Private 
  */
#define MUIA_HTMLview_InstanceData        HTMLview_ID(57)	/* Private */

/**
  * See MUIA_HTMLview_Scrollbars
  */
#define MUIV_HTMLview_Scrollbars_Auto      0

/**
  * See MUIA_HTMLview_Scrollbars
  */
#define MUIV_HTMLview_Scrollbars_Yes       1

/**
  * See MUIA_HTMLview_Scrollbars
  */
#define MUIV_HTMLview_Scrollbars_No        2

/**
  * See MUIA_HTMLview_Scrollbars
  */
#define MUIV_HTMLview_Scrollbars_HorizAuto 3

/* Structures */

struct MUIP_HTMLview_FlushImage
{
	ULONG MethodID;
	STRPTR URL;
};

/** 
  * See MUIM_HTMLview_FlushImage 
  */
#define MUIV_HTMLview_FlushImage_All           0

/** 
  * See MUIM_HTMLview_FlushImage 
  */
#define MUIV_HTMLview_FlushImage_Displayed     1

/** 
  * See MUIM_HTMLview_FlushImage 
  */
#define MUIV_HTMLview_FlushImage_Nondisplayed  2

struct MUIP_HTMLview_GetContextInfo
{
	ULONG MethodID;
	LONG X, Y;
};

struct MUIR_HTMLview_GetContextInfo
{
	STRPTR URL, Target, Img, Frame, Background;
	Object *FrameObj;
	ULONG ImageWidth, ImageHeight, ImageSize, ImageOffsetX, ImageOffsetY;
	STRPTR ImageAltText;
};

struct MUIP_HTMLview_GotoURL
{
	ULONG MethodID;
	STRPTR URL, Target;
};

struct MUIP_HTMLview_AddPart
{
	ULONG MethodID;
	STRPTR File;
};

struct MUIP_HTMLview_VLink
{
	ULONG MethodID;
	STRPTR URL;
};

struct MUIP_HTMLview_Parsed
{
	ULONG MethodID;
	ULONG Parsed;
};

struct MUIP_HTMLview_Search
{
	ULONG MethodID;
	STRPTR String;
	ULONG Flags;
};

/**
  * See MUIM_HTMLview_Search 
  */
#define MUIF_HTMLview_Search_CaseSensitive (1 << 0)

/**
  * See MUIM_HTMLview_Search 
  */
#define MUIF_HTMLview_Search_DOSPattern    (1 << 1)

/**
  * See MUIM_HTMLview_Search 
  */
#define MUIF_HTMLview_Search_Backwards     (1 << 2)

/**
  * See MUIM_HTMLview_Search 
  */
#define MUIF_HTMLview_Search_FromTop       (1 << 3)

/**
  * See MUIM_HTMLview_Search 
  */
#define MUIF_HTMLview_Search_Next          (1 << 4)

struct HTMLview_LoadMsg
{
	LONG lm_Type;

	union
	{
		struct { STRPTR URL; ULONG Flags; } lm_Open;
		struct { STRPTR Buffer; LONG Size; } lm_Read;
		struct { STRPTR Buffer; LONG Size; } lm_Write;
		struct { ; } lm_Close;
	};

	ULONG lm_PageID;
	APTR lm_Userdata;
	Object *lm_App;

	ULONG PostLength;
	STRPTR EncodingType;
};

#define MUIF_HTMLview_LoadMsg_Reload      (1 << 0)
#define MUIF_HTMLview_LoadMsg_Document    (1 << 1)
#define MUIF_HTMLview_LoadMsg_Image       (1 << 2)
#define MUIF_HTMLview_LoadMsg_Post        (1 << 3)
#define MUIF_HTMLview_LoadMsg_Script      (1 << 4)
#define MUIF_HTMLview_LoadMsg_Stylesheet  (1 << 5)
#define MUIF_HTMLview_LoadMsg_MainObject  (1 << 6)

enum
{
	HTMLview_Open = 0,
	HTMLview_Read,
	HTMLview_Close,
	HTMLview_Write
};

#endif
