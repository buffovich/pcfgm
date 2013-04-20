#ifndef PCFGM_TYPES_BASIC
#define PCFGM_TYPES_BASIC

#include <pcfgm/types.h>
#include <stdint.h>

extern int _blob_to_cstr( blob_t *b, char *dst );

extern int _blob_to_int16( blob_t *b, int16_t *dst );

extern int _blob_to_int32( blob_t *b, int32_t *dst )

#endif
