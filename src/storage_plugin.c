typedef struct {
	int fdesc;
	off_t fsize;
	void *mapping;
} bin_storage_t;

methods_table_t _G_methods = {
	[ DESTROY ] = _destroy_config
};

static _destroy_config( int mindex, node_t *node, va_list p ) {
	mixin_t *m = node->head[ mindex ];
	bin_storage_t *bs = ( bin_storage_t* ) &( m->instance.data );
	munmap( bs->mapping, bs->fsize );
	close( bs->fdesc );
}

/*
 * Yes, plugin config is graph too. Mathematically clear.
 * Returns instance of particular initialization.
 */
void* new( node_t *plugin_cfg ) {
	blob_t *v = cfg_value_get( plugin_cfg );
	//TODO: handle case when value is absent
	char *fname = cfg_value_to( v, "cstr" );
	//TODO: handle case when cast is failed

	mixin_t *m = cfg_mixin_alloc( _G_methods, sizeof( bin_storage_t ), 0 );
	bin_storage_t *inst = ( bin_storage_t *) &( m->instance.data );
	
	inst->fdesc = open( fname, O_RDONLY );
	//TODO: handle case when file can't be opened

	struct stat fst;
	fstat( inst->fdesc, &fst );
	inst->fsize = fst->st_size;

	inst->mapping = mmap( NULL, inst->fsize, PROT_READ, MAP_SHARED,
		inst->fdesc, 0
	);

	return m;
}

/*
 * Format:
 * TODO: handle endian issues
 * < options:uint32_t >
 *   node
 *   [ nodes ]
 * node:
 *   < child_num: uint32_t > // alignment - 4 bytes
 *   < blob_t > // alignment - 4 bytes
 *   < names_length: uint32_t > // alignment issues should be taken in account
 *   < char *name > [ < char *name > ... ] // must be padded with extra zeroes
 */

static void *_process_subtree( char *cur_ptr, node_t *n ) {
	uint32_t cnum = *( ( uint32_t* ) cur_ptr );
	cur_ptr += sizeof( uint32_t );
	// OK. If you don't like this ugly name - "advise" me another option
	cfg_node_advise_capacity( n, cnum );

	// postpone set value until intire subtree will be ready
	// it makes sense when you have some underlying modules which
	// placed a hooks on set/get value
	blob_t *datap = ( blob_t* ) cur_ptr;
	
	size_t blob_size = ( datap->options & BLOB_LENGTH_MASK ) *
		( ( datap->options & BLOB_ARRAY ) ? datap->data.length : 1 ) +
		sizeof( uint32_t );
	// daling with alignment
	size_t blob_size_rem = blob_size % sizeof( uint32_t )
	if( blob_size_rem )
		blob_size += sizeof( uint32_t ) - blob_size_rem;
	cur_ptr += blob_size;

	uint32_t names_length = ( uint32_t *) cur_ptr;
	cur_ptr += sizeof( uint32_t );
	char *c = cur_ptr + names_length;
	for( uint32_t i; i < cnum; ++i ) {
		c = _process_subtree( c , cfg_node_create( n, cur_ptr ) );
		cur_ptr += strlen( cur_ptr ) + 1;
	}

	cfg_value_set( n, datap );
	
	/* to protect ourselves from false change detection through mixin hooks
	 * TODO: there is no need to provide any hooks on read-only tree
	 * review this part when there will be a need to write configs
	 * OBSOLETE: since we use mixin in root node to store information about
	 * underlying file and mapping
	 */
	// cfg_mixin_add( n, CAST( inst )->singleton );
	
	return c;
}

/*
 * You should pass start node here. Node should be allocated already
 */
int on_include( void *m, node_t *root ) {
	bin_storage_t *inst = ( bin_storage_t* ) &(
		( ( mixin_t* ) m )->instance.data
	);
	
	uint32_t ptr = ( uint32_t* ) inst->mapping;
	uint32_t options = ( *ptr );
	
	_process_subtree( ptr + 1, root );

	cfg_mixin_add( root, ( mixin_t* ) m );
}
