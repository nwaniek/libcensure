#ifndef __MATHUTIL_H__F496A8C3_62D6_4A5E_80BB_BCB2489559AF
#define __MATHUTIL_H__F496A8C3_62D6_4A5E_80BB_BCB2489559AF

#include "libcensure.h"

struct point {
	int x, y;
};

struct octagon_pts {
	struct point p[8];
};

DLL_LOCAL int octagon_area(const int m, const int n);
DLL_LOCAL void octagon_pts(int m, int n, struct octagon_pts *pts);

#endif /* __MATHUTIL_H__F496A8C3_62D6_4A5E_80BB_BCB2489559AF */

