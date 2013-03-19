#define SOFT_LINK 1

typedef struct {
	unsigned int options;
	union {
		node_t *ptr;
		char path[ 1 ];
	};
} node_pointer_t;

static inline node_t* __get_node( node_t *cur, node_pointer_t *nptr ) {
	return cfg_node_mark(
		( nptr->options & SOFT_LINK ) ?
			cfg_node_get( cur, nptr->path ) :
			nptr->ptr
	);
}

/* Implementation of dumb proxy methods as it's needed for symlink */
static int _get_node( int mindex, node_t *node, const char *name,
	node_t **ret
) {
}

static int _add_node( int mindex, node_t *node, const char *name,
	node_t *what
) {
}

static int _create_node( int mindex, node_t *node, unsigned int options ) {
}

static int _del_node( int mindex, node_t *node, const char *name ) {
}

static int _accept_advice( int mindex, node_t *node, blob_t *value ) {
}

static int _get_iter( int mindex, node_t *node, cfg_iter_t *ret ) {
}

static int _get_value( int mindex, node_t *node, blob_t *value ) {
}

static int _set_value( int mindex, node_t *node, blob_t *value ) {
}

static int _destroy( int mindex, node_t *node ) {
}
/*****************************************************************************/

methods_table_t _pointer_methods = {
	[ GET_ITER ] = _get_iter,
	[ GET_NODE ] = _get_node,
	[ ADD_NODE ] = _add_node,
	[ CREATE_NODE ] = _create_node,
	[ DEL_NODE ] = _del_node,
	[ ACCEPT_ADVICE ] = _accept_advice,
	[ GET_VALUE ] = _get_value,
	[ SET_VALUE ] = _set_value,
	[ DESTROY ] = _destroy
}

#define CAST( v ) ( ( node_pointer_t* ) ( v ) )

void* init( node_t *plugin_cfg ) {
	
}
