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

	utarray_push_back( &( n->classes ), &klass );
	if( ! ( klass->options & CLASS_INSTANCE_HAS_NO_DATA ) )
		utarray_push_back( &( n->head ), &idata );
}

void _cfg_node_make_child( node_t *parent, node_t *child ) {
	assert( child != NULL );
	assert( parent != NULL );
	
	_cfg_node_initialize( child );

	child->parent = parent;

	UT_array *child_classes = &( child->classes ),
		*child_head = &( child->head ),
		*parent_classes = &( parent->classes ),
		*parent_head = &( parent->head );

	utarray_reserve( child_classes, utarray_len( parent_classes ) + 1 );
	utarray_reserve( child_head, utarray_len( parent_head ) + 1 );

	void **cur_data = ( void** )utarray_front( parent_head );
	for(
		class_t **cur_class = ( class_t** )utarray_front( parent_classes );
		cur_class != NULL;
		cur_class = ( class_t** )utarray_next( parent_classes, cur_class )
    )
		if( ! ( ( *cur_class )->options & CLASS_UNINHERITABLE ) ) {
			utarray_push_back( child_classes, cur_class );
			if( ! ( ( *cur_class )->options & CLASS_INSTANCE_HAS_NO_DATA ) ) {
				utarray_push_back( child_head, cur_data );
				cur_data = ( void** )utarray_next( parent_head, cur_data );
			}
		}
}
