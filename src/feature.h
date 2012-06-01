#ifndef __FEATURE_H__FE230802_9183_45DC_A494_7BE085751FE2
#define __FEATURE_H__FE230802_9183_45DC_A494_7BE085751FE2

#include <stdlib.h>
#include <stdbool.h>
#include "libcensure.h"

DLL_LOCAL void features_alloc(struct csx_features **fs, unsigned n);
DLL_LOCAL void features_realloc(struct csx_features *fs, unsigned n);
DLL_LOCAL void features_free(struct csx_features **fs);

#endif /* __FEATURE_H__FE230802_9183_45DC_A494_7BE085751FE2 */

