#include <pcfgm/internal/alloc.h>

#include <stdlib.h>
#include <assert.h>

#define SLOTS_NUM ( sizeof( unsigned int ) * 8 )

#define REF_COUNTER_ALIGN ( alignof( unsigned int ) )

#define REF_COUNTER_SIZE ( sizeof( unsigned int ) )

#define SLAB_ALIGNMENT ( ( sizeof( void* ) > alignof( slab_t ) ) ? \
	sizeof( void* ) : \
	alignof( slab_t ) \
)

#define EMPTY_MAP ( ~ ( 0u ) )

static inline slab_t *__alloc_slab( cache_t *cache, unsigned int nslots ) {
	assert( nslots <= SLOTS_NUM );
	
	// allocating slab with SLOTS_NUM slots
	slab_t **ret;

	posix_memalign( ret,
		SLAB_ALIGNMENT,
		cache->header_sz + cache->blk_sz * nslots
	);

	memset( *ret, 0, sizeof( slab_t ) );
	( *ret )->map = EMPTY_MAP;

	if( nslots < SLOTS_NUM )
		( *ret )->map >>= ( SLOTS_NUM - nslots );

	return *ret;
}

cache_t *_cfg_cache_create( unsigned int options,
	size_t blk_sz,
	unsigned int align,
	unsigned int inum
) {
	// TODO: think about alternative schemes of allocation
	cache_t *c = calloc( 1, sizeof( cache_t ) );
	
	c->options = options;
	// adjust block size to match alignment
	if( blk_sz % align )
		blk_sz += align - ( blk_sz % align );

	if( options & SLAB_REFERABLE ) {
		if( blk_sz % REF_COUNTER_ALIGN )
			blk_sz += REF_COUNTER_ALIGN -
				( blk_sz % REF_COUNTER_ALIGN )

		blk_sz += REF_COUNTER_SIZE;
	}

	c->blk_sz = blk_sz;
	c->align = align;

	// by default we should allocate block with exact total size
	c->header_sz = sizeof( slab_t );
	// then we should consider alignment restrictions and add padding
	if( ( sizeof( slab_t ) ) % align )
		c->header_sz += align - ( ( sizeof( slab_t ) ) % align );

	if( inum > 0 ) {
		slab_t **cur = &( c->head );
		for( int cyc = 0, nbuckets = inum / SLOTS_NUM;
			cyc < nbuckets;
			++cyc
		) {
			*cur = __alloc_slab( c, SLOTS_NUM );
			cur = &( ( *cur )->next );
		}

		// last bucket isn't full
		unsigned int last_bucket_sz = inum % SLOTS_NUM;
		if( last_bucket_sz )
			*cur = __alloc_slab( c, last_bucket_sz );

		c->tail = *cur;
	}

	return c;
}

extern void _cfg_cache_free( cache_t *cache );

extern void _cfg_cache_reap( cache_t *cache );

// mark object as allocated and increment reference number if the case
void *_cfg_object_alloc( cache_t *cache ) {
	void *ret = NULL;
	
	if( cache->head != NULL ) {
		if( cache->head->map )
			ret = __get_block( cache->head,
				ffs( ( int ) cache->head->map ) - 1
			);
		else {
			if( cache->head->next == NULL ) {
				ret = __get_block(
					cache->head = __alloc_slab( cache ),
					0
				);
				cache->head->next = cache->tail;
			} else {
				if( cache->head->next->map ) {
					cache->tail->next = cache->head;
					cache->tail = cache->head;
					cache->head = cache->head->next;
					cache->head->next = cache->tail->next;
					cache->tail->next = NULL;
					ret = __get_block( cache->head,
						ffs( ( int ) cache->head->map ) - 1
					);
				} else {
					slab_t *news = __alloc_slab( cache );
					news->next = cache->head;
					cache->head = news;
					ret = __get_block( news, 0 );
				}
			}
		}
	} else
		ret = __get_block(
			cache->head = cache->tail = __alloc_slab( cache ),
			0
		);

	if( cache->options & SLAB_REFERABLE )
		__reset_refcount( cache, ret );

	return NULL;
}

// increment reference number if the case
extern void *_cfg_object_get( cache_t *cache, void *obj );

// decrement reference number if the case; when number approaches zero then
// object will be marked as free
extern void *_cfg_object_put( cache_t *cache, void *obj );
