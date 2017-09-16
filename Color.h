#ifndef COLOR_H
#define COLOR_H

#include "Global.h"
#include "Image.h"

namespace GrabCutNS {

class Color {

public:

	Color() : r(0), g(0), b(0) {}
	Color(Real _r, Real _g, Real _b) : r(_r), g(_g), b(_b) {}

	Real r, g, b;
};

//计算两个像素点(r,g,b表示)之间的距离的平方
Real distance2( const Color& c1, const Color& c2 );

}
#endif //COLOR_H
