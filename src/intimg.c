#include "intimg.h"


void
intimg_alloc(struct csx_intimg **i, unsigned w, unsigned h)
{
	*i = (struct csx_intimg*)malloc(sizeof(struct csx_intimg));
	(*i)->ii = (unsigned long*)malloc(sizeof(unsigned long) * w * h);
	(*i)->ls = (unsigned long*)malloc(sizeof(unsigned long) * w * h);
	(*i)->rs = (unsigned long*)malloc(sizeof(unsigned long) * w * h);
	(*i)->w = w;
	(*i)->h = h;
}


void
intimg_free(struct csx_intimg **i)
{
	free((*i)->ii);
	free((*i)->ls);
	free((*i)->rs);
	free(*i);
	*i = NULL;
}


void
intimg_compute_fn(struct csx_intimg *i, csx_get_imgdata_fn get, void *p)
{
	if (i == NULL || get == NULL) return;

	const unsigned w = i->w;
	const unsigned h = i->h;

	// first row
	i->ii[0] = i->rs[0] = i->ls[0] = get(p, 0, 0);
	for (unsigned x = 1; x < w; x++) {
		register unsigned long v = get(p, x, 0) + i->ii[x - 1];
		i->ii[x] = i->ls[x] = i->rs[x] = v;
	}

	// other rows
	for (unsigned y = 1, r = w, rr = 0; y < h; y++, r += w, rr += w) {
		register unsigned long sum = 0;
		for (unsigned x = 0; x < w; x++) {
			sum += get(p, x, y);

			i->ii[r + x] = sum + i->ii[rr + x];

			i->ls[r + x] = sum;
			if (x > 0)
				i->ls[r + x] += i->ls[rr + x - 1];

			i->rs[r + x] = sum;
			if (x < (w - 1))
				i->rs[r + x] += i->rs[rr + x + 1];
			else
				i->rs[r + x] += i->rs[rr + x];
		}
	}
}


void
intimg_compute_direct(struct csx_intimg *i, const unsigned char *data, unsigned step)
{
	if (i == NULL || data == NULL) return;

	const unsigned w = i->w;
	const unsigned h = i->h;

	// first row
	i->ii[0] = i->rs[0] = i->ls[0] = (unsigned long)data[0];
	for (unsigned x = 1; x < w; x++) {
		register unsigned long v = (unsigned long)data[x] + i->ii[x - 1];
		i->ii[x] = i->ls[x] = i->rs[x] = v;
	}

	// other rows
	for (unsigned y = 1, r = w, rr = 0; y < h; y++, r += w, rr += w) {
		register unsigned long sum = 0;
		for (unsigned x = 0; x < w; x++) {
			sum += (unsigned long)data[y * step + x];

			i->ii[r + x] = sum + i->ii[rr + x];

			i->ls[r + x] = sum;
			if (x > 0)
				i->ls[r + x] += i->ls[rr + x - 1];

			i->rs[r + x] = sum;
			if (x < (w - 1))
				i->rs[r + x] += i->rs[rr + x + 1];
			else
				i->rs[r + x] += i->rs[rr + x];
		}
	}

}
