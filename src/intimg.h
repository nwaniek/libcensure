#ifndef __INTIMG_H__D04F6AAF_973A_4B76_8E8B_610CE83BD335
#define __INTIMG_H__D04F6AAF_973A_4B76_8E8B_610CE83BD335

#include <stdlib.h>
#include "libcensure.h"

DLL_LOCAL void intimg_alloc(struct csx_intimg **i, unsigned w, unsigned h);
DLL_LOCAL void intimg_free(struct csx_intimg **i);
DLL_LOCAL void intimg_compute_fn(struct csx_intimg *i, csx_get_imgdata_fn get, void *p);
DLL_LOCAL void intimg_compute_direct(struct csx_intimg *i, const unsigned char *data, unsigned step);

#endif /* __INTIMG_H__D04F6AAF_973A_4B76_8E8B_610CE83BD335 */

