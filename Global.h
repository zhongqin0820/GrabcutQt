#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>

//调整算法计算精度
namespace GrabCutNS 
{

#ifdef USE_DOUBLE
	typedef double Real;
#else
	typedef float Real;
#endif



//用户定义 未知区域、前景、背景
enum TrimapValue { TrimapUnknown, TrimapForeground, TrimapBackground };

//粗糙分割得到   分割前景、分割背景
enum SegmentationValue { SegmentationForeground, SegmentationBackground };


//连通图   像素表示    四方向八邻域
//防止重复链接，且允许简单搜索检查
struct NLinks
{
	Real upleft;
	Real up;
	Real upright;
	Real right;
};

//辅助函数  计算像素点间距离
inline Real distance(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	return sqrt((Real)((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)));
}

}//ns

#endif
