#define MINC_NATIVE_ENDIAN 0    /* Use CPU-native format */
#define MINC_BIG_ENDIAN 1       /* Force big-endian */
#define MINC_LITTLE_ENDIAN 2    /* Force little-endian */

typedef void (*minc_swap_fn_t)(char *, size_t);

/**
 * Returns the "right" function pointer for swapping a data type.
 */
extern minc_swap_fn_t minc_get_swap_function(int desired_format, int type_size);
