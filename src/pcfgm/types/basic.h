#ifndef PCFGM_TYPES_BASIC
#define PCFGM_TYPES_BASIC

#include <pcfgm/types.h>
#include <stdint.h>

extern int _blob_to_cstr( blob_t *b, char *dst );

extern int _blob_to_int16( blob_t *b, int16_t *dst );

extern int _blob_to_int32( blob_t *b, int32_t *dst );

static inline int _blob_to_uint32( blob_t *b, uint32_t *dst ) {
	return _blob_to_int32( b, ( int32_t* ) dst );
}

#endif
