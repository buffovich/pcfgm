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
typedef struct {
	mixin_t *singleton;
	int fdesc;
	off_t fsize;
	void *mapping;
} bin_storage_t;

#define CAST( v ) ( ( bin_storage_t* ) ( v ) )
/*
 * Yes, plugin config is graph too. Mathematically clear.
 * Returns instance of particular initialization.
 */
void* init( node_t *plugin_cfg ) {
	blob_t *v = cfg_value_get( plugin_cfg );
	//TODO: handle case when value is absent
	char *fname = cfg_value_to( v, "cstr" );
	//TODO: handle case when cast is failed

	bin_storage_t *inst = ( bin_storage_t *) malloc( sizeof( bin_storage_t ) );
	
	inst->fdesc = open( fname, O_RDONLY );
	//TODO: handle case when file can't be opened

	struct stat fst;
	fstat( inst->fdesc, &fst );
	inst->fsize = fst->st_size;

	inst->mapping = mmap( NULL, inst->fsize, PROT_READ, MAP_SHARED,
		inst->fdesc, 0
	);

	return inst;
}

static void *_process_subtree( char *cur_ptr, node_t *n ) {
	uint32_t cnum = *( ( uint32_t* ) cur_ptr );
	cur_ptr += sizeof( uint32_t );
	// OK. If you don't like this ugly name - "advise" me another option
	cfg_node_advise_for_number_of_children( n, cnum );
	
	blob_t *datap = ( blob_t* ) cur_ptr;
	cfg_value_set( n, datap );
	
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

	cfg_mixin_add( n, CAST( inst )->singleton );
	
	return c;
}

/*
 * You should pass start node here. Node should be allocated already
 */
int on_include( void *inst, node_t *root ) {
	uint32_t ptr = ( uint32_t * ) CAST( inst )->mapping;
	uint32_t options = ( *ptr );
	
	_process_subtree( ptr + 1, root );
}

int close( void *inst ) {
	munmap( CAST( inst )->mapping, CAST( inst )->fsize );
	close( CAST( inst )->fdesc );
	free( inst );
}
