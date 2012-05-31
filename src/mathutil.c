#include "mathutil.h"

int octagon_area(const int m, const int n)
{
	return
		2 * n * n +
		m * (m + 2 * n) +
		2 * m * n;
}


void
octagon_pts(int m, int n, struct octagon_pts *pts)
{
	int m2 = m/2;

	pts->p[0].x = + m2 + n;
	pts->p[0].y = + m2;

	pts->p[1].x = + m2 + n;
	pts->p[1].y = - m2;

	pts->p[2].x = + m2;
	pts->p[2].y = - m2 - n;

	pts->p[3].x = - m2;
	pts->p[3].y = - m2 - n;

	pts->p[4].x = - m2 - n;
	pts->p[4].y = - m2;

	pts->p[5].x = - m2 - n;
	pts->p[5].y = + m2;

	pts->p[6].x = - m2;
	pts->p[6].y = + m2 + n;

	pts->p[7].x = + m2;
	pts->p[7].y = + m2 + n;
}
