#if defined(__MORPHOS__)

#include <cybergraphx/cybergraphics.h>
#include <graphics/rpattr.h>
#include <libraries/mui.h>

#include <proto/alib.h>
#include <proto/cybergraphics.h>
#include <proto/graphics.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include <string.h>

#include "private.h"
//#include <SDI_hook.h>

extern struct Library *CyberGfxBase;

/*#include <proto/exec.h>
#define NewRawDoFmt(__p0, __p1, __p2, ...) \
	(((STRPTR (*)(void *, CONST_STRPTR , APTR (*)(APTR, UBYTE), STRPTR , ...))*(void**)((long)(EXEC_BASE_NAME) - 922))((void*)(EXEC_BASE_NAME), __p0, __p1, __p2, __VA_ARGS__))*/

/***********************************************************************/

#ifndef WritePixelArrayAlpha
#define WritePixelArrayAlpha(__p0, __p1, __p2, __p3, __p4, __p5, __p6, __p7, __p8, __p9) \
    LP10(216, ULONG , WritePixelArrayAlpha, \
        APTR , __p0, a0, \
        UWORD , __p1, d0, \
        UWORD , __p2, d1, \
        UWORD , __p3, d2, \
        struct RastPort *, __p4, a1, \
        UWORD , __p5, d3, \
        UWORD , __p6, d4, \
        UWORD , __p7, d5, \
        UWORD , __p8, d6, \
        ULONG , __p9, d7, \
        , CYBERGRAPHICS_BASE_NAME, 0, 0, 0, 0, 0, 0)
#endif

#define HTML_WIDTH 20
#define HTML_HEIGHT 20
#define HTML_DEPTH 32

unsigned long HTML[] = {
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0b264f9e, 0x204b6dae, 0x395878b4, 0x475373b0, 0x47496aa7, 0x37294988, 0x21143472, 0x0d0e295e, 0x020d275a, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x093359a4, 0x2d5b7bb8, 0x807191c3, 0xbc7f9dca, 0xda90acd3, 0xe393add1, 0xe38aa4c9, 0xd86884b3, 0xc0486599, 0x89284780, 0x4615336e, 0x150e2a61, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x08194397, 0x566484bd, 0xbc82a2cf, 0xfd93b7dc, 0xff99bddd, 0xff92b8db, 0xff92b4d4, 0xff93aec6, 0xff9bb1c6, 0xff8ca3bf, 0xfd637fac, 0xda2c4c89, 0x8c11306d, 0x170d275a, 0x000d2659, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x0a3259a4, 0x586082bc, 0xce82a8d6, 0xfb82b1df, 0xff7bb0df, 0xff7ba9d2, 0xff8aabc8, 0xff9eb7ce, 0xff9ab0c4, 0xff9bb0c3, 0xffa8baca, 0xffa8b9cc, 0xfe7a92b9, 0xf22d4f8f, 0x96102e6a, 0x240e2960, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x2f6082bc, 0xc1779dcf, 0xfb74aadc, 0xff69a6de, 0xff7daeda, 0xff8fb2d1, 0xffaec6db, 0xffcedbe6, 0xffcad8e5, 0xff91b1cd, 0xff7c99b3, 0xff7892ab, 0xff7d95ae, 0xff5d7bab, 0xf112357a, 0x870f2d69, 0x090d275a, 0x00000000, 0x00000000,
	0x0f6280b9, 0x8d7c9fcf, 0xfe7aacdd, 0xff75afe2, 0xff85b7e3, 0xff7ca4c7, 0xff7da0be, 0xffadc4d8, 0xffcdddeb, 0xffbcd3e7, 0xff98b1c9, 0xff89a2b9, 0xff7995ae, 0xff65829d, 0xff587592, 0xff315697, 0xe1123478, 0x5b0e2b64, 0x060d285b, 0x00000000,
	0x305678b5, 0xcd709cd0, 0xff579cda, 0xff4793d6, 0xff4283bc, 0xff48769d, 0xff688dad, 0xff7e9eba, 0xff7799b7, 0xff658bad, 0xff4c7fac, 0xff5b82a6, 0xff7296b7, 0xff7998b6, 0xff486a8b, 0xff436597, 0xf8193d83, 0x9b102f6f, 0x1a0e2a61, 0x00000000,
	0x5c436eb1, 0xef5f96cf, 0xff3d8dd5, 0xff2d7ec6, 0xff2e6697, 0xff5881a5, 0xff6389ab, 0xff3b6b95, 0xff35658f, 0xff34628c, 0xff346c9f, 0xff346a9b, 0xff36648e, 0xff487eb1, 0xff4e7fad, 0xff4a6d98, 0xff244a8e, 0xcd113173, 0x340f2d67, 0x00000000,
	0x7f3b6db2, 0xfc4689cb, 0xff418fd6, 0xff2979c0, 0xff386c99, 0xff5e86a8, 0xff2c618e, 0xff1f5686, 0xff205583, 0xff215481, 0xff225686, 0xff225f96, 0xff22639e, 0xff1864ad, 0xff135fa8, 0xff4a73a1, 0xff2e5598, 0xec113275, 0x490f2e6a, 0x000d295a,
	0x94386cb2, 0xff2d7bc6, 0xff4f97d8, 0xff66a1d7, 0xff80a5c5, 0xff225c8f, 0xff0d508a, 0xff0e497c, 0xff0f4779, 0xff0f4676, 0xff104574, 0xff114778, 0xff0d5394, 0xff045aab, 0xff0053a4, 0xff2863a2, 0xff2d579a, 0xf8113376, 0x550f2e6b, 0x010e275a,
	0x8c3667a9, 0xff276caf, 0xff5094d1, 0xffc1daf2, 0xffaaceef, 0xff177cd5, 0xff0067c2, 0xff014988, 0xff013b6f, 0xff01396c, 0xff013869, 0xff013f79, 0xff0055a4, 0xff0057ab, 0xff0253a3, 0xff2967aa, 0xff285299, 0xf5113275, 0x510f2e6a, 0x010d2659,
	0x72315fa2, 0xf92a65a3, 0xff3d82c0, 0xffa0c7e9, 0xff98c3e9, 0xff2d86d6, 0xff096dc5, 0xff034f93, 0xff043d70, 0xff033a6c, 0xff02396a, 0xff014a8d, 0xff005bb0, 0xff0357aa, 0xff165fa8, 0xff2b67a9, 0xff234c93, 0xe1113274, 0x410f2d69, 0x00000000,
	0x492d5ba2, 0xe42c66a8, 0xff2e76b8, 0xff4f94d3, 0xff3184cf, 0xff4390d5, 0xff3887cf, 0xff1b609f, 0xff0e4475, 0xff0c3f6e, 0xff0f4372, 0xff165997, 0xff0b5aa6, 0xff1862ab, 0xff2266aa, 0xff2861a5, 0xfe1c4289, 0xbb103072, 0x290f2b65, 0x00000000,
	0x202955a1, 0xb42a64ac, 0xff3d80c1, 0xff2377c4, 0xff0667c0, 0xff1773c6, 0xff3281ca, 0xff457fb4, 0xff38648c, 0xff25517b, 0xff47729b, 0xff6c96be, 0xff5788b8, 0xff3473b1, 0xff175da4, 0xff2d5ba0, 0xef14387e, 0x800f2e6a, 0x100e295f, 0x00000000,
	0x071b4698, 0x692c62ab, 0xf53d7abd, 0xff1b6ebd, 0xff0664ba, 0xff106cbf, 0xff1a71c2, 0xff2a72b4, 0xff3b6993, 0xff446a8e, 0xff8badcd, 0xffbed3e7, 0xff97b6d6, 0xff2d6cad, 0xff2665a8, 0xff1b428b, 0xcd113172, 0x390d285c, 0x0407142d, 0x00000000,
	0x00000000, 0x192451a0, 0x923266ad, 0xf22068b3, 0xff1265b5, 0xff166ab9, 0xff1f70bc, 0xff2773ba, 0xff2b69a2, 0xff2c6ca9, 0xff74a2cf, 0xff9ab9d9, 0xff5c8dbf, 0xff2b67a9, 0xff224f97, 0xd80f2f6d, 0x5b0d2658, 0x09000103, 0x08000000, 0x07000000,
	0x00000000, 0x040e2b62, 0x351e4488, 0xae295ea2, 0xf12f6eb2, 0xff2e74b9, 0xff2f76b9, 0xff3a7cbd, 0xff4884bf, 0xff578dc3, 0xff6796c5, 0xff497fb7, 0xff336aa9, 0xf9235092, 0xde143574, 0x670c2454, 0x1a051027, 0x0d000000, 0x0b000000, 0x06000000,
	0x00000000, 0x06000000, 0x14000102, 0x48112953, 0x9b2d528a, 0xe7517cb3, 0xfc5988be, 0xff6896c6, 0xff719cc9, 0xff6693c3, 0xff346bac, 0xfc2a5b9b, 0xea1f447f, 0xb5112b5e, 0x6a091b3f, 0x1a000204, 0x0f000000, 0x09000000, 0x04000000, 0x00000000,
	0x00000000, 0x05000000, 0x10000000, 0x20010307, 0x3908142b, 0x6d2f456a, 0x943f5b88, 0xb5436497, 0xc13b6098, 0xc12c548d, 0xb1204276, 0x9317315d, 0x6b0c1e40, 0x41051026, 0x2001050b, 0x0d000000, 0x05000000, 0x01000000, 0x00000000, 0x00000000,
	0x00000000, 0x01000000, 0x03000000, 0x08000000, 0x0f000000, 0x1a010204, 0x26040c1c, 0x33081632, 0x380a1938, 0x38081733, 0x2f050f22, 0x23020710, 0x15010103, 0x0c000000, 0x06000000, 0x01000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

/***********************************************************************/

static
ULONG mAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
    DoSuperMethodA(cl, obj, (Msg)msg);

    msg->MinMaxInfo->MinWidth  = HTML_WIDTH+4;
    msg->MinMaxInfo->MinHeight = HTML_HEIGHT;
    msg->MinMaxInfo->MaxWidth  = HTML_WIDTH+4;
    msg->MinMaxInfo->MaxHeight = HTML_HEIGHT;
    msg->MinMaxInfo->DefWidth  = HTML_WIDTH+4;
    msg->MinMaxInfo->DefHeight = HTML_HEIGHT;

    return 0;
}

/***********************************************************************/

static ULONG
mDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
    DoSuperMethodA(cl, obj, (Msg)msg);

    if (msg->flags & (MADF_DRAWOBJECT|MADF_DRAWUPDATE))
		WritePixelArrayAlpha(HTML,0,0,HTML_WIDTH<<2,_rp(obj),_mleft(obj)+4,_mtop(obj),HTML_WIDTH,HTML_HEIGHT,0xffffffff);

    return 0;
}

/***********************************************************************/


#define MUIM_Layout                         0x8042845b /* V4  */
struct  MUIP_Layout                         { ULONG MethodID; LONG left; LONG top; LONG width; LONG height; ULONG flags; };

static ULONG
mLayout(struct IClass *cl,Object *obj,struct MUIP_Layout *msg)
{
	//DoSuperMethodA(cl,obj,(Msg)msg);

	NewRawDoFmt("--- Layout %ld %ld %ld %ld %lx\n",(void * (*)(void *, UBYTE))1,NULL ,
	msg->left,
	msg->top,
	msg->width,
	msg->height,
	msg->flags);

	msg->left   = _left(obj);
	msg->top    = _top(obj);
	msg->width  = _width(obj);
	msg->height = _height(obj);

	return 1;
}

/***********************************************************************/

DISPATCHER(PicDispatcher)
{
    switch (msg->MethodID)
    {
        case MUIM_AskMinMax: return mAskMinMax(cl,obj,(struct MUIP_AskMinMax *)msg);
        case MUIM_Draw: 	 return mDraw(cl,obj,(struct MUIP_Draw *)msg);
        //case MUIM_Layout:    return mLayout(cl,obj,(struct MUIP_Layout *)msg);
        case OM_NEW:
		{
            ULONG res;
            res = DoSuperMethodA(cl, obj, msg);

			//NewRawDoFmt("--- OM_NEW %lx\n",(void * (*)(void *, UBYTE))1,NULL,res);
            return res;
        }
    }

    return DoSuperMethodA(cl, obj, msg);
}

/***********************************************************************/

struct MUI_CustomClass *PicClass = NULL;

ULONG
initPicClass(void)
{
    return (ULONG)(PicClass = MUI_CreateCustomClass(NULL,MUIC_Area,NULL,0,ENTRY(PicDispatcher)));
}

/***********************************************************************/


#endif
