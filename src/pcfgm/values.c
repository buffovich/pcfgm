#include <pcfgm/values.h>

#include <stdint.h>
#include <byteswap.h>

static inline void __swap_bytes( void *where, size_t len ) {
	char *limit = where + len;
	for( char *to = where, *from = limit - 1;
		cyc < limit;
		cyc += len
	)
		*to ^= *from ^= *to ^= *from;
}

static inline void __copy( blob_t *from, void *to ) {
	if( b->options & BLOB_ARRAY )
		memcpy( to,
			from->array.data,
			from->array.length * ( from->options & BLOB_LENGTH_MASK )
		)
	else
		memcpy( to,
			from->value,
			from->options & BLOB_LENGTH_MASK
		);
}

void cfg_blob_copy( blob_t *from, void *to ) {
	size_t len = from->options & BLOB_LENGTH_MASK;
	
	if( to == NULL ) {
		if( from->options & BLOB_ARRAY )
			return ( len * from->array.length )
		else
			return len;
	}
	
	if( is_host_endian( from ) ||
		( ( from->options & BLOB_LENGTH_MASK ) == 1 )
	)
		__copy( from, to );
	else {
		__copy( from, to );
		if( from->options & BLOB_ARRAY )
			switch( len ) {
				case 2:
					for( char *limit = to + 2 * from->array.length, *cyc = to;
						cyc < limit;
						cyc += 2
					)
						bswap_16( *( ( uint16_t* ) cyc ) );
					break;
				case 4:
					for( char *limit = to + 4 * from->array.length, *cyc = to;
						cyc < limit;
						cyc += 4
					)
						bswap_32( *( ( uint32_t* ) cyc ) );
					break;
				default:
					for( char *limit = to + len * from->array.length, *cyc = to;
						cyc < limit;
						cyc += len
					)
						__swap_bytes( cyc, len );
			}
		else
			switch( len ) {
				case 2:
					bswap_16( *( ( uint16_t* ) to ) );
					break;
				case 4:
					bswap_32( *( ( uint32_t* ) to ) );
					break;
				default:
					__swap_bytes( to, len );
			};
	}
}
