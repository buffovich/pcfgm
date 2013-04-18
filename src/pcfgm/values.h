/* Operations with node itself */

extern int cfg_value_set( cfg_node_t node,
	blob_t *value
);

extern blob_t *cfg_value_get( cfg_node_t node );

// TODO: continue definition
#define CFG_BLOB_TO( blobptr, type, vptr )
	_blob_to_ ## type( ( blobptr ), ( vptr ) )

#define CFG_NODE_TO( node, type, vptr ) \
	CFG_BLOB_TO( cfg_value_get( node ), ( type ), ( vptr ) )

#define CFG_VALUE( node, path, type, vptr ) \
	CFG_NODE_TO( cfg_node_lookup( ( node ), ( path ) ), ( type ), ( vptr ) )

// make self pointer from blob double pointer
#define CFG_PPTR_TO_SELF( pp, mtype ) ( mtype ) *self = \
	( ( mtype )* ) ( *( pp ) );
