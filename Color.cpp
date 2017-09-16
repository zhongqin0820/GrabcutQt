#include "Color.h"

namespace GrabCutNS {

//计算两个颜色点(r,g,b表示)之间的距离的平方
Real distance2( const Color& c1, const Color& c2 )
{
	return ((c1.r-c2.r)*(c1.r-c2.r)+(c1.g-c2.g)*(c1.g-c2.g)+(c1.b-c2.b)*(c1.b-c2.b));
}

}
