#include <api/plugin.h>
#include <stdarg.h>

/* feature of storage plugin is that entire file with binary config tree
 * is mapped to memory
 */
typedef struct {
	off_t fsize; /* size of file (mapping) */
	void *mapping; /* pointer to mapping */
	int fdesc; /* file destriptor for binary */
} bin_storage_t;

/* we need only cleanup ourselves */
static class_t _G_storage = {
	.options = CLASS_UNINHERITABLE,
	.vtable = {
		[ DESTROY ] = _destroy_config
	}
};

/* de-mapping and file closing */
static int _destroy_config( node_t *node,
	class_t **klass,
	void **datapp,
	va_list args
) {
	CFG_PPTR_TO_SELF( datapp, bin_storage_t )

	munmap( self->mapping, self->fsize );
	close( self->fdesc );

	// TODO: take a look to alternative allocation routines
	free( self );

	// Please, framework, invoke the next destructor. OK?
	return 1;
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

static void *__process_subtree( char *cur_ptr, node_t *n ) {
	uint32_t cnum = *( ( uint32_t* ) cur_ptr );
	cur_ptr += sizeof( uint32_t );
	// OK. If you don't like this ugly name - "advise" me another option
	_cfg_node_advise_capacity( n, cnum );

	// postpone set value until entire subtree will be ready
	// it makes sense when you have some underlying modules which
	// placed a hooks on set/get value
	blob_t *datap = ( blob_t* ) cur_ptr;

	_cfg_value_set( n, datap );

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
	node_t *cnode;
	for( uint32_t i; i < cnum; ++i ) {
		cnode = cfg_node_create( n );
		c = __process_subtree( c, cnode );
		_cfg_node_link( n, cnode, cur_ptr );
		cur_ptr += strlen( cur_ptr ) + 1;
	}

	/* to protect ourselves from false change detection through mixin hooks
	 * TODO: there is no need to provide any hooks on read-only tree
	 * review this part when there will be a need to write configs
	 * OBSOLETE: since we use mixin in root node to store information about
	 * underlying file and mapping
	 * TODO: will be needed during implementation of writable config
	 */
	// cfg_mixin_add( n, CAST( inst )->singleton );
	
	return c;
}

node_t* on_create( node_t* cfg, node_t* me ) {
	char *fname = CFG_NODE_VALUE_TO(
		_cfg_node_lookup( cfg, "path" ),
		"cstr"
	);

	if( fname == NULL )
		return NULL;
	//TODO: handle case when node is absent
	//TODO: handle case when value is absent
	//TODO: handle case when cast is failed

	//TODO: think about alternative allocator
	bin_storage_t *self = calloc( 1, sizeof( bin_storage_t ) );

	self->fdesc = open( fname, O_RDONLY );
	//TODO: handle case when file can't be opened

	struct stat fst;
	fstat( self->fdesc, &fst );
	self->fsize = fst->st_size;

	uint32_t ptr =
		self->mapping = mmap( NULL, self->fsize, PROT_READ, MAP_SHARED,
			self->fdesc, 0
		);

	uint32_t options = ( *ptr );

	_cfg_node_add_class( me,
		&_G_storage,
		self
	);
	
	__process_subtree( ptr + 1, me );

	// always successfull first run of binding
	return me;
}
