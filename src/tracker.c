#include "tracker.h"
#include "descriptor.h"
#include <float.h>

#define MAX(A, B) ((A) > (B) ? (A) : (B))


void
match_table_alloc(struct csx_match_table **t, unsigned n)
{
	(*t) = malloc(sizeof(**t));
	if (n) {
		(*t)->l = (unsigned *)malloc(sizeof(unsigned) * n);
		(*t)->r = (unsigned *)malloc(sizeof(unsigned) * n);
	}
	else {
		(*t)->l = NULL;
		(*t)->r = NULL;
	}
	(*t)->n = 0;
	(*t)->capacity = n;
}


void
match_table_realloc(struct csx_match_table *t, unsigned n)
{
	t->capacity = n;
	t->l = (unsigned*)realloc(t->l, sizeof(unsigned) * n);
	t->r = (unsigned*)realloc(t->r, sizeof(unsigned) * n);
}


void
match_table_free(struct csx_match_table **t)
{
	if ((*t)->capacity) {
		free((*t)->l);
		free((*t)->r);
	}
	free(*t);
	*t = NULL;
}

static inline void
swap(float * restrict x, float * restrict y)
{
	float tmp = *x;
	*x = *y;
	*y = tmp;
}

void
track_features(const struct csx_features *fs1, const struct csx_surf64_descriptors *ds1,
		const struct csx_features *fs2, const struct csx_surf64_descriptors *ds2,
		struct csx_match_table *t)
{
	if (!fs1 || !fs2 || !ds1 || !ds2) {
		if (t) t->n = 0;
		return;
	}
	t->n = 0;
	if (!fs1->n || !fs2->n)
		for (unsigned i = 0; i < fs1->n; i++)
			fs1->f[i].mtable_id = -1;
	else {
		for (unsigned i = 0; i < fs1->n; i++) {

			int r = -1;
			float d1 = FLT_MAX;
			float d2 = FLT_MAX;

			for (unsigned j = 0; j < fs2->n; j++) {
				float d = surf64_distance(&ds1->d[i], &ds2->d[j]);

				if (d < d1) {
					d2 = d1;
					d1 = d;
					r = j;
				}
				else if (d < d2)
					d2 = d;
			}

			// XXX
			// this is according to the OpenSURF C++ implementation,
			// the reference to the paper describing this value is
			// missing, though
			if (d1 / d2 < 0.65) {
				if (t->n >= t->capacity)
					match_table_realloc(t, t->capacity + 128);

				fs1->f[i].mtable_id = t->n;
				t->l[t->n] = i;
				t->r[t->n] = r;
				t->n++;
			}
			else
				fs1->f[i].mtable_id = -1;
		}
	}
}


void
track_features_nn(const struct csx_features *fs1, const struct csx_surf64_descriptors *ds1,
		const struct csx_features *fs2, const struct csx_surf64_descriptors *ds2,
		struct csx_match_table *t)
{
	if (!fs1 || !fs2 || !ds1 || !ds2) {
		if (t) t->n = 0;
		return;
	}
	t->n = 0;
	if (!fs1->n || !fs2->n)
		for (unsigned i = 0; i < fs1->n; i++)
			fs1->f[i].mtable_id = -1;
	else {
		for (unsigned i = 0; i < fs1->n; i++) {
			int r = 0;
			float d1 = surf64_distance(&ds1->d[i], &ds2->d[0]);

			for (unsigned j = 1; j < fs2->n; j++) {
				float d = surf64_distance(&ds1->d[i], &ds2->d[j]);
				if (d < d1) {
					d1 = d;
					r = j;
				}
			}

			if (d1 < 0.4) {
				if (t->n >= t->capacity)
					match_table_realloc(t, t->capacity + 128);

				fs1->f[i].mtable_id = t->n;
				t->l[t->n] = i;
				t->r[t->n] = r;
				t->n++;
			}
			else
				fs1->f[i].mtable_id = -1;
		}
	}
}

