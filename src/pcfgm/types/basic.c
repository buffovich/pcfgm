#include <pcfgm/types/basic.h>

#include <pcfgm/values.h>
#include <pcfgm/types.h>
#include <pcfgm/internal/err.h>

#include <stdint.h>

int _blob_to_cstr( blob_t *b, char *dst ) {
	if( ! is_cstr( b ) ) {
		_cfg_fire_error( CFG_TYPE_UNAPPLICABLE );
		return -1;
	}

	return cfg_blob_copy( b, dst );
}

int _blob_to_int16( blob_t *b, int16_t *dst ) {
	if( ( b->options & ( BLOB_ARRAY | BLOB_FLOAT ) ) ||
		!( b->options & BLOB_SIGNED )
		( ( b->options & BLOB_LENGTH_MASK ) != sizeof( int16_t ) )
	) {
		_cfg_fire_error( CFG_TYPE_UNAPPLICABLE );
		return -1;
	}

	return cfg_blob_copy( b, dst );
}

int _blob_to_int32( blob_t *b, int32_t *dst ) {
	if( ( b->options & ( BLOB_ARRAY | BLOB_FLOAT ) ) ||
		!( b->options & BLOB_SIGNED )
		( ( b->options & BLOB_LENGTH_MASK ) != sizeof( int32_t ) )
	) {
		_cfg_fire_error( CFG_TYPE_UNAPPLICABLE );
		return -1;
	}

	return cfg_blob_copy( b, dst );
}
