#ifndef __LIBCENSURE_H__4E5ABA8C_275C_400C_9E73_A1E4F6373372
#define __LIBCENSURE_H__4E5ABA8C_275C_400C_9E73_A1E4F6373372

#if defined _WIN32 || defined __CYGWIN__
#  ifdef BUILDING_DLL
#    ifdef __GNUC__
#      define DLL_PUBLIC __attribute__ ((dllexport))
#    else
#      define DLL_PUBLIC __declspec(dllexport)
#    endif
#  else
#    ifdef __GNUC__
#      define DLL_PUBLIC __attribute__ ((dllimport))
#    else
#      define DLL_PUBLIC __declspec(dllimport)
#    endif
#  endif
#  define DLL_LOCAL
#else
#  if __GNUC__ >= 4
#    define DLL_PUBLIC __attribute__ ((visibility ("default")))
#    define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#  else
#    define DLL_PUBLIC
#    define DLL_LOCAL
#  endif
#endif

#define CSX_SUCCESS                    0
#define CSX_E_INVALID_FTYPE            1
#define CSX_E_INVALID_SETUP            2
#define CSX_E_MALLOC_FAILED            3
#define CSX_E_INVALID_DETECTION_RESULT 4

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	CSX_FT_BOX,
	CSX_FT_OCT,
	CSX_MAX_FILTER_TYPES
} csx_filter_type_t;

typedef struct csx_setup csx_setup_t;
typedef unsigned (*csx_get_imgdata_fn)(void *p, unsigned x, unsigned y);

struct csx_box_param
{
	unsigned scale;
	int n;
	int inner_length;
	int outer_length;
	int inner_half;
	int outer_half;
	int inner_area;
	int outer_area;
	int border;
	float inner_weight;
	float outer_weight;
};

struct csx_oct_param
{
	unsigned scale;
	int m[2];
	int n[2];
	int inner_area;
	int outer_area;
	int border;
	float inner_weight;
	float outer_weight;
};

union csx_uparam {
	struct csx_box_param box;
	struct csx_oct_param oct;
};

struct csx_filter_params
{
	csx_filter_type_t ftype;
	unsigned nscales;
	int *margins;
	union csx_uparam *params;
};

struct csx_intimg
{
	unsigned w, h;
	unsigned long *ii, *ls, *rs;
};


struct csx_surf64 {
	float v[64];
};


struct csx_surf64_descriptors {
	struct csx_surf64 *d;
	unsigned n;
	unsigned capacity;
};


struct csx_feature
{
	unsigned x, y;
	unsigned scale;

	/*
	 * this field is an 'injection' from the feature tracking: it is used to
	 * make correspondance selection using the match_table faster. it's
	 * value is either -1 if there is no match or >= 0 for an index to the
	 * match_table .l and .r fields. this value is only set after a call to
	 * track_features and may contain rubbish otherwise.
	 */
	int mtable_id;
};


struct csx_features {
	struct csx_feature *f;
	unsigned n;
	unsigned capacity;
};


struct csx_detection_result {
	struct csx_intimg *ii;
	struct csx_features *fs;
	struct csx_surf64_descriptors *ds;
	struct csx_filter_params *fparams;
};


struct csx_match_table
{
	unsigned *l;
	unsigned *r;
	unsigned n;
	unsigned capacity;
};





DLL_PUBLIC unsigned csx_initialize(csx_filter_type_t ftype, unsigned width, unsigned height, csx_setup_t **setup);
DLL_PUBLIC unsigned csx_finalize(csx_setup_t **setup);

DLL_PUBLIC unsigned csx_detect(csx_setup_t *setup, csx_get_imgdata_fn fn, void *img, struct csx_detection_result **dr);
DLL_PUBLIC unsigned csx_track(struct csx_detection_result *left,
		struct csx_detection_result *right,
		struct csx_match_table **mt);

DLL_PUBLIC unsigned csx_free_detection_result(struct csx_detection_result **dr);
DLL_PUBLIC unsigned csx_free_match_table (struct csx_match_table **mt);

#ifdef __cplusplus
}
#endif

#endif /* __LIBCENSURE_H__4E5ABA8C_275C_400C_9E73_A1E4F6373372 */

