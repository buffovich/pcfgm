typedef struct {
	mixin_t *singleton;
	void *mapping;
} bin_storage_t;

#define CAST( v ) ( ( bin_storage_t* ) ( v ) )
/*
 * Yes, plugin config is graph too. Mathematically clear.
 * Returns instance of particular initialization.
 */
void* init( node_t *plugin_cfg ) {
}

static void *_process_subtree( void *cur_ptr, node_t *pred, int num_succ ) {
	node_t n = cfg_node_add( pred, name );
	for ( _process_subtree
	cfg_mixin_add( pred, CAST( inst )->singleton );
}

/*
 * You should pass start node here. Node should be allocated already
 */
int on_include( void *inst, node_t *root ) {
	
	
	cfg_value_set( node, blob_p );
}

int close( void *inst ) {
}
