#include <pcfgm/internal/graph.h>

#include <pcfgm/types.h>

#include <pcfgm/internal/utils.h>

#include <utarray.h>
#include <assert.h>

static UT_icd _G_class_icd = { sizeof( class_t* ), NULL, NULL, NULL };
static UT_icd _G_ptr_icd = { sizeof( void* ), NULL, NULL, NULL };



void _cfg_node_initialize( node_t *n ) {
	assert( n != NULL );
	
	memset( n, 0, sizeof( node_t ) );
	utarray_init( &( n->classes ), &_G_class_icd );
	utarray_init( &( n->head ), &_G_ptr_icd );
}

void _cfg_node_finalize( node_t *n ) {
	assert( n != NULL );

	utarray_done( &( n->classes ) );
	utarray_done( &( n->head ) );
}

void _cfg_node_add_class( node_t *n, class_t *klass, void *idata ) {
	assert( n != NULL );
	assert( klass != NULL );
	assert( ( klass->options & CLASS_INSTANCE_HAS_NO_DATA ) ?
		( idata == NULL ) :
		( idata != NULL )
	);

	
}
