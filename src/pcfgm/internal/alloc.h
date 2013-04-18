#ifndef API_ALLOC
#define API_ALLOC

extern slab_t *cfg_slab_create( size_t bsize, size_t align, size_t inum );
extern int cfg_slab_free( slab_t *slab );

extern class_t *cfg_class_alloc( methods_table_t methods,
	unsigned int options,
	class_t *prev
);

extern int cfg_class_free( class_t *m );

extern node_t *cfg_node_alloc( node_t *parent, class_t *mix_with );

extern int cfg_node_free( node_t *which );

extern blob_t *cfg_blob_alloc( unsigned int options,
	/* if == 0 then pointer alignment will be applied */
	size_t data_align,
	size_t idata_len // if array
);

// function valid for arrays
extern blob_t *cfg_blob_realloc( blob_t *which,
	size_t idata_len
);

// if blob is shared then do nothing
extern int cfg_blob_free( blob_t *b );

#endif
