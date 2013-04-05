#include <api/plugin.h>

/* feature of storage plugin is that entire file with binary config tree
 * is mapped to memory
 */
typedef struct {
	int fdesc; /* file destriptor for binary */
	off_t fsize; /* size of file (mapping) */
	void *mapping; /* pointer to mapping */
} bin_storage_t;

/* we need only cleanup ourselves */
static methods_table_t _G_methods = {
	[ DESTROY ] = _destroy_config
};

/* de-mapping and file closing */
static _destroy_config( int mindex, node_t *node, va_list p ) {
	CFG_NODE_TO_SELF( node, mindex, bin_storage_t );

	munmap( self->mapping, self->fsize );
	close( self->fdesc );
}

void* new( node_t *plugin_cfg ) {
	char *fname = CFG_NODE_VALUE_TO( plugin_cfg, "cstr" );
	//TODO: handle case when value is absent
	//TODO: handle case when cast is failed

	mixin_t *m = cfg_mixin_alloc( _G_methods, sizeof( bin_storage_t ), 0 );
	CFG_MIXIN_TO_SELF( m, bin_storage_t );
	
	self->fdesc = open( fname, O_RDONLY );
	//TODO: handle case when file can't be opened

	struct stat fst;
	fstat( self->fdesc, &fst );
	self->fsize = fst->st_size;

	self->mapping = mmap( NULL, self->fsize, PROT_READ, MAP_SHARED,
		self->fdesc, 0
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

	// postpone set value until entire subtree will be ready
	// it makes sense when you have some underlying modules which
	// placed a hooks on set/get value
	blob_t *datap = ( blob_t* ) cur_ptr;
	
	size_t blob_size = ( datap->options & BLOB_LENGTH_MASK ) *
		( ( datap->options & BLOB_ARRAY ) ? datap->data.length : 1 ) +
		sizeof( uint32_t );
	// dealing with alignment
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

node_t* try_create( void *m, node_t *parent ) {
	CFG_MIXIN_TO_SELF( ( mixin_t* ) m, bin_storage_t );
	
	uint32_t ptr = ( uint32_t* ) self->mapping;
	uint32_t options = ( *ptr );

	node_t *me = cfg_node_produce( parent );
	
	_process_subtree( ptr + 1, me );

	cfg_mixin_add( me, ( mixin_t* ) m );

	// always successfull first run of binding
	return me;
}
