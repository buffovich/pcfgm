/* Operations with node itself */

extern int cfg_value_set( cfg_node_t node,
	blob_t *value
);

extern blob_t * cfg_value_get( cfg_node_t node );

extern blob_t *cfg_blob_alloc( unsigned int options,
	size_t length // if array
)

// if blob is shared then do nothing
extern int cfg_blob_free( blob_t *b );

// TODO: continue definition
#define cfg_value_to( blob, type )


extern mixin_t *cfg_mixin_alloc( method_table_t *methods
	size_t idata_len
);

extern int cfg_mixin_add( node_t *node,
	mixin_t *mixin
)

extern int cfg_mixin_del( node_t *node,
	int num
)
