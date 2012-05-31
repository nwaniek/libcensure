#include "detector.h"
#include <math.h>


bool
is_local_extremum(float *src, int step, int w, int n)
{
	float u = fabsf(src[0]);

	for (int y = -n; y <= n; y++) {
		int r = y * w;
		for (int x = -n; x <= n; x++) {

			for (int z = 1; z <= n/2; z++) {
				if (u <= fabsf(src[-z * step + r + x]))
					return false;
				if (u <= fabsf(src[+z * step + r + x]))
					return false;
			}

			if ((x || y) && (u <= fabsf(src[r + x])))
				return false;
		}
	}

	return true;
}


float
harris_cornerness(float *src, int w, int extents)
{
	float l11, l12, l22, dx, dy;
	l11 = l12 = l22 = dx = dy = 0.f;

	for (int y = -extents; y <= extents; y++) {
		for (int x = -extents; x <= extents; x++) {

			dy = src[(y + 1) * w + x]
			   - src[(y - 1) * w + x];

			dx = src[y * w + x + 1]
			   - src[y * w + x - 1];

			l11 = l11 + dy * dy;
			l12 = l12 + dx * dy;
			l22 = l22 + dx * dx;
		}
	}

	// trace(H) / det(H)
	return ((l11 + l22) * (l11 + l22)) / (l11 * l22 - l12 * l12);
}


void
detect_features(struct csx_features *fs, float *scale_space, unsigned nscales, int w,
	int h, int *margins)
{
	const float local_threshold = 30.f;
	const float harris_threshold = 10.f;

	int step = w * h;
	float *dst = &scale_space[step];
	for (unsigned scale = 1; scale < nscales - 1; scale++, dst += step) {
		int border = margins[scale + 1];

		for (int y = border; y < h - border; y++) {
			int r = y * w;
			for (int x = border; x < w - border; x++) {
				float v = fabsf(dst[r + x]);
				if (v < local_threshold)
					continue;

				if (!is_local_extremum(&dst[r + x], step, w, 3))
					continue;

				if (harris_cornerness(&dst[r + x], w, margins[scale]) > harris_threshold)
					continue;


				// dynamically allocate more memory chunks
				if (fs->n >= fs->capacity)
					features_realloc(fs, fs->capacity + 128);

				fs->f[fs->n].x = x;
				fs->f[fs->n].y = y;
				fs->f[fs->n++].scale = scale;
			}
		}
	}
}
