#include "descriptor.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

/*
 * lookup tables for gaussian values, normalized for the respective size
 * gauss_25 is a 9x9 lookup table for a gaussian with sigma = 2.5
 * gauss_15 is a 4x4 lookup table for a gaussian with sigma = 1.5
 *
 * Example:
 *	gauss_25[7][2] == 0.0104059
 */
DLL_LOCAL
const float gauss_25[9][9] = {
	{0.0022759, 0.0039843, 0.0059439, 0.0075562, 0.0081855, 0.0075562, 0.0059439, 0.0039843, 0.0022759},
	{0.0039843, 0.0069753, 0.0104059, 0.0132284, 0.0143302, 0.0132284, 0.0104059, 0.0069753, 0.0039843},
	{0.0059439, 0.0104059, 0.0155237, 0.0197345, 0.0213782, 0.0197345, 0.0155237, 0.0104059, 0.0059439},
	{0.0075562, 0.0132284, 0.0197345, 0.0250875, 0.0271770, 0.0250875, 0.0197345, 0.0132284, 0.0075562},
	{0.0081855, 0.0143302, 0.0213782, 0.0271770, 0.0294405, 0.0271770, 0.0213782, 0.0143302, 0.0081855},
	{0.0075562, 0.0132284, 0.0197345, 0.0250875, 0.0271770, 0.0250875, 0.0197345, 0.0132284, 0.0075562},
	{0.0059439, 0.0104059, 0.0155237, 0.0197345, 0.0213782, 0.0197345, 0.0155237, 0.0104059, 0.0059439},
	{0.0039843, 0.0069753, 0.0104059, 0.0132284, 0.0143302, 0.0132284, 0.0104059, 0.0069753, 0.0039843},
	{0.0022759, 0.0039843, 0.0059439, 0.0075562, 0.0081855, 0.0075562, 0.0059439, 0.0039843, 0.0022759}};

DLL_LOCAL
const float gauss_15[4][4] = {
	{0.0381582, 0.0595124, 0.0595124, 0.0381582},
	{0.0595124, 0.0928170, 0.0928170, 0.0595124},
	{0.0595124, 0.0928170, 0.0928170, 0.0595124},
	{0.0381582, 0.0595124, 0.0595124, 0.0381582}};


// force creation of the symbol
extern inline float surf64_distance(struct csx_surf64 *a, struct csx_surf64 *b);

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

DLL_LOCAL float
haar_x(const struct csx_intimg *ii, int x, int y, int s)
{
	/*
	 *      |   |   |
	 *    F | D | B |
	 *   ------------
	 *      |   |   |
	 *      |   |   |
	 *    E | C | A |
	 *   ---|---|---|
	 *
	 *   x1 - x2 = (A - B - C + D) - (C - D - E + F)
	 *           = A - B - 2C + 2D + E - F
	 */

	const int w = (int)ii->w;
	const int h = (int)ii->h;
	long A, B, C, D, E, F;
	int x1, y1, y2;

	A = B = C = D = E = F = 0;

	x1 = MIN(x + s - 1, w - 1);
	y1 = MIN(y + s - 1, h - 1);
	y2 = MIN(y - s - 1, h - 1);

	if (x1 >= 0 && y1 >= 0) A = ii->ii[x1 + y1 * w];
	if (x1 >= 0 && y2 >= 0) B = ii->ii[x1 + y2 * w];

	x1 = MIN(x - 1, w - 1);
	if (x1 >= 0 && y1 >= 0) C = ii->ii[x1 + y1 * w];
	if (x1 >= 0 && y2 >= 0) D = ii->ii[x1 + y2 * w];

	x1 = MIN(x - s - 1, w - 1);
	if (x1 >= 0 && y1 >= 0) E = ii->ii[x1 + y1 * w];
	if (x1 >= 0 && y2 >= 0) F = ii->ii[x1 + y2 * w];

	return (float)(A - B - 2 * C + 2 * D + E - F);
}

DLL_LOCAL float
haar_y(const struct csx_intimg *ii, int x, int y, int s)
{
	/*
	 *
	 *      |       |
	 *    F |   E   |
	 *   ------------
	 *    D |   B   |
	 *   ---|-------|
	 *    C |   A   |
	 *   ---|-------|
	 *
	 *   x1 = A - B - C + D
	 *   x2 = B - E - D + F
	 *
	 *   x1 - x2 = (A - B - C + D) - (B - E - D + F)
	 *           = (A - 2B - C + 2D + E - F
	 */

	const int w = (int)ii->w;
	const int h = (int)ii->h;
	long A, B, C, D, E, F;
	int x1, x2, y1;

	A = B = C = D = E = F = 0;

	x1 = MIN(x + s - 1, w - 1);
	x2 = MIN(x - s - 1, w - 1);
	y1 = MIN(y + s - 1, h - 1);

	if (x1 >= 0 && y1 >= 0) A = ii->ii[x1 + y1 * w];
	if (x2 >= 0 && y1 >= 0) C = ii->ii[x2 + y1 * w];

	y1 = MIN(y - 1, h - 1);
	if (x1 >= 0 && y1 >= 0) B = ii->ii[x1 + y1 * w];
	if (x2 >= 0 && y1 >= 0) D = ii->ii[x2 + y1 * w];

	y1 = MIN(y - s - 1, h - 1);
	if (x1 >= 0 && y1 >= 0) E = ii->ii[x1 + y1 * w];
	if (x2 >= 0 && y1 >= 0) F = ii->ii[x2 + y1 * w];

	return (float)(A - 2 * B - C + 2 * D + E - F);
}


#define MAKE_STORE_FN(NAME, OFFSET, LX_INC, LY_INC, G2_X, G2_Y) \
	DLL_LOCAL void \
	store_to_q##NAME (struct csx_surf64 *d, int lx, int ly, float dx, float dy) \
	{ \
		const int wx  = lx + LX_INC; \
		const int wy  = ly + LY_INC; \
		const float g = gauss_25[wx][wy]; \
		dx = g * dx; \
		dy = g * dy; \
		\
		d->v[OFFSET + 0] += dx; \
		d->v[OFFSET + 1] += fabsf(dx); \
		d->v[OFFSET + 2] += dy; \
		d->v[OFFSET + 3] += fabsf(dy); \
	}

/*
 * Q0 ... Q3
 */
MAKE_STORE_FN(0,  0, 12, 12, 0, 0)
MAKE_STORE_FN(1,  4, 12,  7, 0, 1)
MAKE_STORE_FN(2,  8, 12,  2, 0, 2)
MAKE_STORE_FN(3, 12, 12, -3, 0, 3)

/*
 * Q4 ... Q7
 */
MAKE_STORE_FN(4, 16,  7, 12, 1, 0)
MAKE_STORE_FN(5, 20,  7,  7, 1, 1)
MAKE_STORE_FN(6, 24,  7,  2, 1, 2)
MAKE_STORE_FN(7, 28,  7, -3, 1, 3)

/*
 * Q8 ... QB
 */
MAKE_STORE_FN(8, 32,  2, 12, 2, 0)
MAKE_STORE_FN(9, 36,  2,  7, 2, 1)
MAKE_STORE_FN(A, 40,  2,  2, 2, 2)
MAKE_STORE_FN(B, 44,  2, -3, 2, 3)

/*
 * QC ... QF
 */
MAKE_STORE_FN(C, 48, -3, 12, 3, 0)
MAKE_STORE_FN(D, 52, -3,  7, 3, 1)
MAKE_STORE_FN(E, 56, -3,  2, 3, 2)
MAKE_STORE_FN(F, 60, -3, -3, 3, 3)


DLL_LOCAL void
store_dxdy(struct csx_surf64 *d, const int lx, const int ly, const float dx,
	const float dy)
{

	/* select the correct quadrant to store dx,dy to. the overlap makes it
	 * quite ugly
	 *
	 * 0 4 8 C
	 * 1 5 9 D
	 * 2 6 A E
	 * 3 7 B F
	 *
	 */

	if (lx < -3) {
		if (ly < -3)
			store_to_q0(d, lx, ly, dx, dy);
		if (ly >= -7 && ly < 2)
			store_to_q1(d, lx, ly, dx, dy);
		if (ly >= -2 && ly < 7)
			store_to_q2(d, lx, ly, dx, dy);
		if (ly >= 3)
			store_to_q3(d, lx, ly, dx, dy);
	}
	if (lx >= -7 && lx < 2) {
		if (ly < -3)
			store_to_q4(d, lx, ly, dx, dy);
		if (ly >= -7 && ly < 2)
			store_to_q5(d, lx, ly, dx, dy);
		if (ly >= -2 && ly < 7)
			store_to_q6(d, lx, ly, dx, dy);
		if (ly >= 3)
			store_to_q7(d, lx, ly, dx, dy);
	}
	if (lx >= -2 && lx < 7) {
		if (ly < -3)
			store_to_q8(d, lx, ly, dx, dy);
		if (ly >= -7 && ly < 2)
			store_to_q9(d, lx, ly, dx, dy);
		if (ly >= -2 && ly < 7)
			store_to_qA(d, lx, ly, dx, dy);
		if (ly >= 3)
			store_to_qB(d, lx, ly, dx, dy);
	}
	if (lx >= 3) {
		if (ly < -3)
			store_to_qC(d, lx, ly, dx, dy);
		if (ly >= -7 && ly < 2)
			store_to_qD(d, lx, ly, dx, dy);
		if (ly >= -2 && ly < 7)
			store_to_qE(d, lx, ly, dx, dy);
		if (ly >= 3)
			store_to_qF(d, lx, ly, dx, dy);
	}
}


void
create_descriptor(const struct csx_feature *f, const struct csx_intimg *ii,
	struct csx_surf64 *d)
{
	// TODO: check if the struct shouldn't be changed to contain ints!
	const int px = (int)f->x;
	const int py = (int)f->y;
	const int s = (int)f->scale;

	memset(d->v, 0, sizeof(float) * 64);

	for (int ly = -12; ly < 12; ly++) {
		int y = py + ly * s;
		for (int lx = -12; lx < 12; lx++) {
			int x = px + lx * s;

			float dx = haar_x(ii, x, y, s);
			float dy = haar_y(ii, x, y, s);

			store_dxdy(d, lx, ly, dx, dy);
		}
	}

	// gaussian weighting (+ sum extraction)
	float sum = 0.0f;
	int offset = 0;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++, offset += 4) {
			const float g = gauss_15[i][j];

			d->v[offset + 0] *= g;
			d->v[offset + 1] *= g;
			d->v[offset + 2] *= g;
			d->v[offset + 3] *= g;

			sum += d->v[offset + 0] * d->v[offset + 0]
			     + d->v[offset + 1] * d->v[offset + 1]
			     + d->v[offset + 2] * d->v[offset + 2]
			     + d->v[offset + 3] * d->v[offset + 3];
		}


	// normalization
	sum = sqrtf(sum);
	for (int i = 0; i < 64; i++)
		d->v[i] /= sum;
}




void
surf64_compute_descriptors(const struct csx_features *fs, const struct csx_intimg *ii,
	struct csx_surf64_descriptors *ds)
{
	if (ds->capacity < fs->n)
		surf64_descriptors_realloc(ds, fs->n);

	for (unsigned i = 0; i < fs->n; i++)
		create_descriptor(&fs->f[i], ii, &ds->d[i]);
}


void
surf64_alloc(struct csx_surf64 **ds, unsigned n)
{
	*ds = malloc(sizeof(**ds) * n);
}


void
surf64_free(struct csx_surf64 **ds)
{
	free(*ds);
	*ds = NULL;
}


void
surf64_descriptors_alloc(struct csx_surf64_descriptors **ds, unsigned n)
{
	*ds = malloc(sizeof(**ds));
	if (n)
		(*ds)->d = malloc(sizeof(*(*ds)->d) * n);
	else
		(*ds)->d = NULL;
	(*ds)->n = 0;
	(*ds)->capacity = n;
}


void
surf64_descriptors_realloc(struct csx_surf64_descriptors *ds, unsigned n)
{
	ds->capacity = n;
	ds->d = realloc(ds->d, sizeof(*ds->d) * n);
}


void
surf64_descriptors_free(struct csx_surf64_descriptors **ds)
{
	if ((*ds)->capacity)
		free((*ds)->d);
	free(*ds);
	*ds = NULL;
}

