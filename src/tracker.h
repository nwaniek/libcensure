#ifndef __TRACKER_H__B00C01C7_5F66_4B6B_BB28_0D3C02CC1800
#define __TRACKER_H__B00C01C7_5F66_4B6B_BB28_0D3C02CC1800

#include "feature.h"
#include "descriptor.h"

/**
 * void track_features - track features from two different feature sets.
 *
 * searches for features within fs1 and fs2 which have only a small distance to
 * each other. the result will be stored to the match_table, where a match-table
 * entry's l value will be the index in fs1 and its r value the index in fs2.
 * additionally, fs1's feature entries will get their mtable_id set to the
 * match table id.
 * fs2 entries will not be altered, making it possible to keep tracking
 * information over time.
 * if a feature in fs1 was not found in fs2, its mtable_id will be -1
 *
 * TODO: decide whether to write fs2 as well or not.
 *
 */
DLL_LOCAL void track_features(const struct csx_features *fs1,
		const struct csx_surf64_descriptors *ds1,
		const struct csx_features *fs2,
		const struct csx_surf64_descriptors *ds2,
		struct csx_match_table *t);

DLL_LOCAL void track_features_nn(const struct csx_features *fs1,
		const struct csx_surf64_descriptors *ds1,
		const struct csx_features *fs2,
		const struct csx_surf64_descriptors *ds2,
		struct csx_match_table *t);

DLL_LOCAL void match_table_alloc(struct csx_match_table **t, unsigned n);
DLL_LOCAL void match_table_realloc(struct csx_match_table *t, unsigned n);
DLL_LOCAL void match_table_free(struct csx_match_table **t);

#endif /* __TRACKER_H__B00C01C7_5F66_4B6B_BB28_0D3C02CC1800 */

