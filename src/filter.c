#include "filter.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "mathutil.h"


unsigned
setup_box_filter_params(struct csx_filter_params *ptr)
{
	const unsigned n_max = 11;

	ptr->ftype = CSX_FT_BOX;
	ptr->nscales = n_max;
	ptr->margins = malloc(sizeof(int) * n_max);
	ptr->params = malloc(sizeof(union csx_uparam) * n_max);
	if (!ptr->margins || !ptr->params)
		return CSX_E_MALLOC_FAILED;

	for (unsigned i = 0, n = 1; i < n_max; i++, n++) {
		ptr->params[i].box.scale = i;
		ptr->params[i].box.n = n;
		ptr->params[i].box.inner_length = n * 2 + 1;
		ptr->params[i].box.outer_length = n * 4 + 1;
		ptr->params[i].box.inner_half   = n;
		ptr->params[i].box.outer_half   = n * 2;
		ptr->params[i].box.border       = n * 2;
		ptr->params[i].box.inner_area   = (n * 2 + 1) * (n * 2 + 1);
		ptr->params[i].box.outer_area   = (n * 4 + 1) * (n * 4 + 1);
		ptr->params[i].box.inner_weight = 1.f / ptr->params[i].box.inner_area;
		ptr->params[i].box.outer_weight = 1.f / (ptr->params[i].box.outer_area - ptr->params[i].box.inner_area);

		ptr->margins[i] = ptr->params[i].box.border;
	}

	return CSX_SUCCESS;
}


unsigned
setup_oct_filter_params(struct csx_filter_params *ptr)
{
	const unsigned n_max = 7;
	const int m[][2] = {{3,5}, {3,5}, {3,7}, {5,9}, {5,9}, {5,13}, {5,15}};
	const int n[][2] = {{0,2}, {1,3}, {2,3}, {2,4}, {3,7}, {4,7}, {5,10}};

	ptr->ftype = CSX_FT_OCT;
	ptr->nscales = n_max;
	ptr->margins = malloc(sizeof(int) * n_max);
	ptr->params = malloc(sizeof(*ptr->params) * n_max);
	if (!ptr->margins || !ptr->params)
		return CSX_E_MALLOC_FAILED;

	for (unsigned i = 0; i < n_max; i++) {
		ptr->params[i].oct.scale = i;
		ptr->params[i].oct.m[INNER] = m[i][INNER];
		ptr->params[i].oct.m[OUTER] = m[i][OUTER];
		ptr->params[i].oct.n[INNER] = n[i][INNER];
		ptr->params[i].oct.n[OUTER] = n[i][OUTER];
		ptr->params[i].oct.inner_area = octagon_area(m[i][INNER], n[i][INNER]);
		ptr->params[i].oct.outer_area = octagon_area(m[i][OUTER], n[i][OUTER]);
		ptr->params[i].oct.border   = m[i][OUTER]/2 + n[i][OUTER];
		ptr->params[i].oct.inner_weight = 1.f / ptr->params[i].oct.inner_area;
		ptr->params[i].oct.outer_weight = 1.f / (ptr->params[i].oct.outer_area - ptr->params[i].oct.inner_area);

		ptr->margins[i] = ptr->params[i].oct.border;
	}

	return CSX_SUCCESS;
}


// TODO: Free space on error
unsigned
filter_params_copy(struct csx_filter_params **dest, struct csx_filter_params *src)
{
	*dest = malloc(sizeof(**dest));
	if (!*dest)
		return CSX_E_MALLOC_FAILED;

	(*dest)->ftype = src->ftype;
	(*dest)->nscales = src->nscales;
	(*dest)->margins = malloc(sizeof(int) * src->nscales);
	(*dest)->params = malloc(sizeof(*(*dest)->params) * src->nscales);
	if (!(*dest)->margins || !(*dest)->params)
		return CSX_E_MALLOC_FAILED;

	memcpy((*dest)->margins, src->margins, sizeof(int) * src->nscales);
	memcpy((*dest)->params, src->params, sizeof(*src->params) * src->nscales);

	return CSX_SUCCESS;
}


unsigned
filter_params_create(csx_filter_type_t ftype, struct csx_filter_params **ptr)
{
	*ptr = malloc(sizeof(**ptr));
	if (!*ptr)
		return CSX_E_MALLOC_FAILED;

	unsigned result = CSX_SUCCESS;
	switch (ftype) {
	case CSX_FT_BOX:
		result = setup_box_filter_params(*ptr);
		break;

	case CSX_FT_OCT:
		result = setup_oct_filter_params(*ptr);
		break;

	default:;
	}

	if (result != CSX_SUCCESS)
		filter_params_free(ptr);
	return result;
}




void
box_filter_compute_response(float *dst, const struct csx_intimg *ii,
	struct csx_box_param *param)
{
	if (!dst || !ii || !param) return;

	int w = (int)ii->w;
	int h = (int)ii->h;
	int border = param->border;

	float inner_w = param->inner_weight;
	float outer_w = param->outer_weight;
	int inner_h = param->inner_half;
	int outer_h = param->outer_half;

	long A, B, C, D;
	long inner, outer;
	int x1, x2, y1, y2;

	for (int y = border; y < h - border; y++) {
		const int r = y * w;
		for (int x = border; x < w - border; x++) {
			// inner rectangle
			A = B = C = D = 0;
			x1 = x + inner_h;
			x2 = x - inner_h - 1;
			y1 = y + inner_h;
			y2 = y - inner_h - 1;
			if (x1 >= 0 && y1 >= 0) A = (long)ii->ii[x1 + y1 * w];
			if (x1 >= 0 && y2 >= 0) B = (long)ii->ii[x1 + y2 * w];
			if (x2 >= 0 && y1 >= 0) C = (long)ii->ii[x2 + y1 * w];
			if (x2 >= 0 && y2 >= 0) D = (long)ii->ii[x2 + y2 * w];
			inner = A - B - C + D;

			// outer rectangle
			A = B = C = D = 0;
			x1 = x + outer_h;
			x2 = x - outer_h - 1;
			y1 = y + outer_h;
			y2 = y - outer_h - 1;
			if (x1 >= 0 && y1 >= 0) A = (long)ii->ii[x1 + y1 * w];
			if (x1 >= 0 && y2 >= 0) B = (long)ii->ii[x1 + y2 * w];
			if (x2 >= 0 && y1 >= 0) C = (long)ii->ii[x2 + y1 * w];
			if (x2 >= 0 && y2 >= 0) D = (long)ii->ii[x2 + y2 * w];
			outer = A - B - C + D - inner;

			// write
			float tmp = inner_w * (float)inner - outer_w * (float)outer;
			dst[r + x] = tmp;
		}
	}
}



void
oct_filter_compute_response(float *dst, const struct csx_intimg *ii,
	struct csx_oct_param *param)
{
	if (!dst || !ii || !param) return;

	int w = (int)ii->w;
	int h = (int)ii->h;
	int border = param->border;

	float inner_w = param->inner_weight;
	float outer_w = param->outer_weight;

	long A, B, C, D;
	long inner, outer;

	int x1, x2, y1, y2;

	for (int y = border; y < h - border; y++) {
		const int r = y * w;
		for (int x = border; x < w - border; x++) {

			// inner : rectangle
			A = B = C = D = 0;
			x1 = x + param->m[INNER]/2 + param->n[INNER];
			y1 = y + param->m[INNER]/2;
			x2 = x - param->m[INNER]/2 - param->n[INNER] - 1;
			y2 = y - param->m[INNER]/2 - 1;

			if (x1 >= 0 && y1 >= 0) A = (long)ii->ii[x1 + y1 * w];
			if (x1 >= 0 && y2 >= 0) B = (long)ii->ii[x1 + y2 * w];
			if (x2 >= 0 && y1 >= 0) C = (long)ii->ii[x2 + y1 * w];
			if (x2 >= 0 && y2 >= 0) D = (long)ii->ii[x2 + y2 * w];

			inner = A - B - C + D;

			// inner: lower trapezoid (reuse x1, y1)
			A = B = C = D = 0;
			x2 = x + param->m[INNER]/2;
			y2 = y + param->m[INNER]/2 + param->n[INNER];

			if (x1 >= 0 && y1 >= 0) B = (long)ii->rs[x1 + y1 * w];
			if (x2 >= 0 && y2 >= 0) A = (long)ii->rs[x2 + y2 * w];

			x1 = x - param->m[INNER]/2 - 1;
			if (x1 >= 0 && y2 >= 0) C = (long)ii->ls[x1 + y2 * w];

			x2 = x - param->m[INNER]/2 - param->n[INNER] - 1;
			if (x2 >= 0 && y1 >= 0) D = (long)ii->ls[x2 + y1 * w];

			inner += A - B - C + D;

			// inner : upper trapezoid
			A = B = C = D = 0;
			x1 = x + param->m[INNER]/2 + param->n[INNER] - 1;
			y1 = y - param->m[INNER]/2 - 1;
			if (x1 >= 0 && y1 >= 0) A = (long)ii->ls[x1 + y1 * w];

			x1 = x - param->m[INNER]/2 - param->n[INNER];
			if (x1 >= 0 && y1 >= 0) C = (long)ii->rs[x1 + y1 * w];

			y2 = y - param->m[INNER]/2 - param->n[INNER] - 1;
			x1 = x + param->m[INNER]/2 - 1;
			if (x1 >= 0 && y2 >= 0) B = (long)ii->ls[x1 + y2 * w];

			x1 = x - param->m[INNER]/2;
			if (x1 >= 0 && y2 >= 0) D = (long)ii->rs[x1 + y2 * w];

			inner += A - B - C + D;


			// outer : rectangle
			A = B = C = D = 0;
			x1 = x + param->m[OUTER]/2 + param->n[OUTER];
			y1 = y + param->m[OUTER]/2;
			x2 = x - param->m[OUTER]/2 - param->n[OUTER] - 1;
			y2 = y - param->m[OUTER]/2 - 1;

			if (x1 >= 0 && y1 >= 0) A = (long)ii->ii[x1 + y1 * w];
			if (x1 >= 0 && y2 >= 0) B = (long)ii->ii[x1 + y2 * w];
			if (x2 >= 0 && y1 >= 0) C = (long)ii->ii[x2 + y1 * w];
			if (x2 >= 0 && y2 >= 0) D = (long)ii->ii[x2 + y2 * w];

			outer = A - B - C + D;

			// outer : lower trapezoid
			A = B = C = D = 0;
			x2 = x + param->m[OUTER]/2;
			y2 = y + param->m[OUTER]/2 + param->n[OUTER];

			if (x1 >= 0 && y1 >= 0) B = (long)ii->rs[x1 + y1 * w];
			if (x2 >= 0 && y2 >= 0) A = (long)ii->rs[x2 + y2 * w];

			x1 = x - param->m[OUTER]/2 - 1;
			if (x1 >= 0 && y2 >= 0) C = (long)ii->ls[x1 + y2 * w];

			x2 = x - param->m[OUTER]/2 - param->n[OUTER] - 1;
			if (x2 >= 0 && y1 >= 0) D = (long)ii->ls[x2 + y1 * w];

			outer += A - B - C + D;

			// outer : upper trapezoid
			A = B = C = D = 0;
			x1 = x + param->m[OUTER]/2 + param->n[OUTER] - 1;
			y1 = y - param->m[OUTER]/2 - 1;
			if (x1 >= 0 && y1 >= 0) A = (long)ii->ls[x1 + y1 * w];

			x1 = x - param->m[OUTER]/2 - param->n[OUTER];
			if (x1 >= 0 && y1 >= 0) C = (long)ii->rs[x1 + y1 * w];

			y2 = y - param->m[OUTER]/2 - param->n[OUTER] - 1;
			x1 = x + param->m[OUTER]/2 - 1;
			if (x1 >= 0 && y2 >= 0) B = (long)ii->ls[x1 + y2 * w];

			x1 = x - param->m[OUTER]/2;
			if (x1 >= 0 && y2 >= 0) D = (long)ii->rs[x1 + y2 * w];

			outer += A - B - C + D;

			outer -= inner;

			dst[r + x] = inner_w * (float)inner - outer_w * (float)outer;
		}
	}
}


void
filter_compute_response(float *dst, const struct csx_intimg *ii, struct csx_filter_params *p, unsigned scale)
{
	if (!dst || !ii || !p) return;

	switch (p->ftype) {
	case CSX_FT_BOX:
		box_filter_compute_response(dst, ii, &p->params[scale].box);
		break;

	case CSX_FT_OCT:
		oct_filter_compute_response(dst, ii, &p->params[scale].oct);
		break;

	default:;
	}
}


void
filter_params_free(struct csx_filter_params **p)
{
	if (!*p || !p) return;

	if ((*p)->params) free((*p)->params);
	if ((*p)->margins) free((*p)->margins);
	free(*p);
}


