#ifndef AREACLASS_H
#define AREACLASS_H

#include "AttrClass.h"

// forward declarations

class AreaClass : public AttrClass
{
	public:
		AreaClass () : AttrClass() { ; }
		~AreaClass ()
		{
			delete URL;
			delete Target;
			if(Shape == Area_Poly)
				delete Poly.Coords;
		}
		VOID Parse (REG(a2, struct ParseMessage &pmsg));
		BOOL UseMap (struct UseMapMessage &umsg);

	protected:
		STRPTR URL, Target;
		union
		{
			struct { UWORD Left, Top, Right, Bottom; } Rect;
			struct { struct CoordList *Coords; } Poly;
			struct { UWORD CenterX, CenterY, Radius; } Circle;
		};
		UWORD Shape;
};

#endif // AREACLASS_H
