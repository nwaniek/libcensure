#include "feature.h"


void
features_alloc(struct csx_features **fs, unsigned n)
{
	(*fs) = malloc(sizeof(**fs));
	if (n)
		(*fs)->f = malloc(sizeof(*(*fs)->f) * n);
	else
		(*fs)->f = NULL;

	(*fs)->n = 0;
	(*fs)->capacity = n;
}


void
features_realloc(struct csx_features *fs, unsigned n)
{
	fs->capacity = n;
	fs->f = realloc(fs->f, sizeof(*fs) * n);
}


void
features_free(struct csx_features **fs)
{
	if ((*fs)->capacity)
		free((*fs)->f);
	free(*fs);
	*fs = NULL;
}

