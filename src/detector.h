#ifndef __DETECTOR_H__9E0B5AA5_D71A_4933_B65D_A4661A866D38
#define __DETECTOR_H__9E0B5AA5_D71A_4933_B65D_A4661A866D38

#include <stdbool.h>
#include "feature.h"

DLL_LOCAL bool is_local_extremum(float *src, int step, int w, int n);
DLL_LOCAL float harris_cornerness(float *src, int w, int extents);
DLL_LOCAL void detect_features(struct csx_features *fs, float *scale_space, unsigned nscales,
		int w, int h, int *margins);

#endif /* __DETECTOR_H__9E0B5AA5_D71A_4933_B65D_A4661A866D38 */

