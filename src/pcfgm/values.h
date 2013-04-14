/* Operations with node itself */

extern int cfg_value_set( cfg_node_t node,
	blob_t *value
);

extern blob_t *cfg_value_get( cfg_node_t node );

// TODO: continue definition
#define CFG_VALUE_TO( blob, type )

#define CFG_NODE_VALUE_TO( node, type ) \
	CFG_VALUE_TO( cfg_value_get( node ), ( type ) )

#define CFG_VALUE_BY_PATH( node, path, type ) \
	CFG_NODE_VALUE_TO( cfg_node_lookup( ( node ), ( path ) ), ( type ) )

// make self pointer from blob double pointer
#define CFG_PPTR_TO_SELF( pp, mtype ) ( mtype ) *self = \
	( ( mtype )* ) ( *( pp ) );
