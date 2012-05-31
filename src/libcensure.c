#include "libcensure.h"

#include "feature.h"
#include "intimg.h"
#include "detector.h"
#include "descriptor.h"
#include "tracker.h"
#include "filter.h"
#include <string.h>
#include <stdio.h>

struct csx_setup
{
	csx_filter_type_t ftype;
	struct csx_filter_params *params;
	float *scale_space;
	unsigned width;
	unsigned height;
};


unsigned
csx_initialize(csx_filter_type_t ftype, unsigned width, unsigned height, struct csx_setup **setup)
{
	switch (ftype) {
	case CSX_FT_BOX:
	case CSX_FT_OCT:
		break;
	default:
		return CSX_E_INVALID_FTYPE;
	}

	*setup = malloc(sizeof(**setup));
	(*setup)->ftype = ftype;
	(*setup)->width = width;
	(*setup)->height = height;

	unsigned result = filter_params_create(ftype, &(*setup)->params);
	if (result != CSX_SUCCESS) {
		csx_finalize(setup);
		return result;
	}

	(*setup)->scale_space = malloc(sizeof(float) * width * height * (*setup)->params->nscales);
	if (!(*setup)->scale_space) {
		csx_finalize(setup);
		return CSX_E_MALLOC_FAILED;
	}

	return CSX_SUCCESS;
}


unsigned
csx_finalize(struct csx_setup **setup)
{
	if (setup && *setup) {
		filter_params_free(&(*setup)->params);
		free((*setup)->scale_space);
		free(*setup);
		*setup = NULL;
	}
	return CSX_SUCCESS;
}


// TODO: check intermediary results in order to invalidate the detection
// result on error
unsigned
csx_detect(struct csx_setup *setup, csx_get_imgdata_fn fn, void *img,
		struct csx_detection_result **dr)
{
	if (!setup)
		return CSX_E_INVALID_SETUP;

	// memory management
	if (!*dr || !dr) {
		*dr = malloc(sizeof(**dr));
		memset(*dr, 0, sizeof(**dr));
	}
	if (!(*dr)->ii)
		intimg_alloc(&(*dr)->ii, setup->width, setup->height);
	if (!(*dr)->fs)
		features_alloc(&(*dr)->fs, 0);
	else
		(*dr)->fs->n = 0;
	if (!(*dr)->ds)
		surf64_descriptors_alloc(&(*dr)->ds, 0);
	else
		(*dr)->ds->n = 0;
	if (!(*dr)->fparams)
		filter_params_copy(&(*dr)->fparams, setup->params);

	// filtering
	intimg_compute_fn((*dr)->ii, fn, img);
	struct csx_filter_params *p = setup->params;
	for (unsigned scale = 0; scale < p->nscales; scale++)
		filter_compute_response(&setup->scale_space[setup->width * setup->height * scale],
				(*dr)->ii, p, scale);

	// detection + description
	detect_features((*dr)->fs, setup->scale_space, setup->params->nscales,
			setup->width, setup->height, setup->params->margins);
	surf64_compute_descriptors((*dr)->fs, (*dr)->ii, (*dr)->ds);

	return CSX_SUCCESS;
}


unsigned
csx_track(struct csx_detection_result *left,
		struct csx_detection_result *right,
		struct csx_match_table **mt)
{
	if (!left || !right) return CSX_E_INVALID_DETECTION_RESULT;

	// memory management
	if (!*mt || !mt)
		match_table_alloc(mt, 0);
	else
		(*mt)->n = 0;

	// tracking
	track_features(left->fs, left->ds, right->fs, right->ds, *mt);

	return CSX_SUCCESS;
}


unsigned
csx_free_detection_result(struct csx_detection_result **dr)
{
	if (!*dr || !dr) return CSX_E_INVALID_DETECTION_RESULT;

	if ((*dr)->ii) intimg_free(&(*dr)->ii);
	if ((*dr)->ds) surf64_descriptors_free(&(*dr)->ds);
	if ((*dr)->fs) features_free(&(*dr)->fs);
	free(*dr);
	*dr = NULL;
	dr = NULL;

	return CSX_SUCCESS;
}


unsigned
csx_free_match_table (struct csx_match_table **mt)
{
	if (mt && *mt) match_table_free(mt);
	return CSX_SUCCESS;
}



