#ifndef __FILTERS_H__23871A91_67CA_4FC0_913B_428A0A958602
#define __FILTERS_H__23871A91_67CA_4FC0_913B_428A0A958602

#include <stdlib.h>
#include "intimg.h"
#include "libcensure.h"

enum {
	INNER = 0,
	OUTER = 1
};


// TODO: need return values for all functions

// generic functions
DLL_LOCAL unsigned filter_params_create(csx_filter_type_t ftype, struct csx_filter_params **ptr);
DLL_LOCAL unsigned filter_params_copy(struct csx_filter_params **dest, struct csx_filter_params *src);
DLL_LOCAL void filter_params_free(struct csx_filter_params **p);
DLL_LOCAL void filter_compute_response(float *dst, const struct csx_intimg *ii, struct csx_filter_params *p, unsigned scale);

// specific function
DLL_LOCAL unsigned setup_box_filter_params(struct csx_filter_params *ptr);
DLL_LOCAL unsigned setup_oct_filter_params(struct csx_filter_params *ptr);
DLL_LOCAL void box_filter_compute_response(float *dst, const struct csx_intimg *ii, struct csx_box_param *param);
DLL_LOCAL void oct_filter_compute_response(float *dst, const struct csx_intimg *ii, struct csx_oct_param *param);


#endif /* __FILTERS_H__23871A91_67CA_4FC0_913B_428A0A958602 */

