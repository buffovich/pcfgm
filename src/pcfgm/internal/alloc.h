#ifndef PCFGM_INTERNAL_ALLOC
#define PCFGM_INTERNAL_ALLOC

#defined SLAB_REFERABLE 1

typedef struct {
	unsigned int nrefs;
	uint8_t bytes[];
} ref_buffer_t;

typedef uint8_t buffer_t[];

typedef struct {
	slab_t *next;
	// bitmap of free and occupied blocks
	unsigned int map;
} slab_t;

typedef struct {
	// applied alignment in data block
	unsigned int align;
	// element size
	size_t el_sz;
	slab_t *head;
	slab_t *tail;
} cache_t;

extern cache_t *_cfg_cache_create( unsigned int options,
	size_t el_sz,
	unsigned int align,
	unsigned int inum
);

extern void _cfg_cache_free( cache_t *cache );

extern void _cfg_cache_reap( cache_t *cache );

// mark object as allocated and increment reference number if the case
extern void *_cfg_object_alloc( cache_t *slab );

// increment reference number if the case
extern void *_cfg_object_get( cache_t *slab, void *obj );

// decrement reference number if the case; when number approaches zero then
// object will be marked as free
extern void *_cfg_object_put( cache_t *slab, void *obj );

#endif
