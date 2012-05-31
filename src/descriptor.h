#ifndef __DESCRIPTOR_H__268A6E30_EE60_4FB6_9DFB_14A1EF747E5B
#define __DESCRIPTOR_H__268A6E30_EE60_4FB6_9DFB_14A1EF747E5B

#include <stdlib.h>
#include <math.h>
#include <libcensure.h>
#include "feature.h"
#include "intimg.h"


/*
 * euclidean distance between two surf64 descriptors
 */
DLL_LOCAL inline float
surf64_distance(struct csx_surf64 *a, struct csx_surf64 *b)
{
	float r = 0.f;
	for (int i = 0; i < 64; i++) {
		float v = (a->v[i] - b->v[i]);
		r += v * v;
	}
	return sqrtf(r);
}


DLL_LOCAL void surf64_alloc(struct csx_surf64 **ds, unsigned n);
DLL_LOCAL void surf64_free(struct csx_surf64 **ds);

DLL_LOCAL void surf64_descriptors_alloc(struct csx_surf64_descriptors **ds, unsigned n);
DLL_LOCAL void surf64_descriptors_realloc(struct csx_surf64_descriptors *ds, unsigned n);
DLL_LOCAL void surf64_descriptors_free(struct csx_surf64_descriptors **ds);

DLL_LOCAL void create_descriptor(const struct csx_feature *f, const struct csx_intimg *ii, struct csx_surf64 *d);


/**
 * compute Modified Upright SURF descriptors (MU-SURF). this will automatically
 * extent the memory used by ds when required
 */
DLL_LOCAL void surf64_compute_descriptors(const struct csx_features *fs,
		const struct csx_intimg *ii, struct csx_surf64_descriptors *ds);

#endif /* __DESCRIPTOR_H__268A6E30_EE60_4FB6_9DFB_14A1EF747E5B */

