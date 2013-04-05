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
#define CFG_VALUE_TO( blob, type )

#define CFG_NODE_VALUE_TO( node, type ) \
	CFG_VALUE_TO( cfg_value_get( node ), ( type ) )

// make this pointer from mixin pointer
#define CFG_MIXIN_TO_SELF( mixin, mtype ) ( mtype ) *self = \
	( ( mtype )* ) &( ( mixin )->instance.data )

// make this pointer from node pointer
#define CFG_NODE_TO_SELF( node, mindex, mtype ) \
	CFG_MIXIN_TO_THIS( ( node )->head[ ( mindex ) ], mtype )


extern mixin_t *cfg_mixin_alloc( method_table_t *methods,
	size_t idata_len,
	/* if == 0 then pointer alignment will be applied */
	size_t data_align
);

extern int cfg_mixin_realloc( mixin_t *m, size_t idata_len );

extern int cfg_mixin_free( mixin_t *m );

extern int cfg_mixin_add( node_t *node, mixin_t *mixin );

extern int cfg_mixin_del( node_t *node, int num );
