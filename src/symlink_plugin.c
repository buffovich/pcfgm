#include <api/plugin.h>

/* be careful ! Soft links will work much slower then hard ones.*/
/* now, think about situation when we are referencing to the branch
 * which is not accessible yet but it will at the end of pass 1.
 * Probably, we need pass #2?
 *
 * Here is the idea:
 * we enqueue nodes/plugins which report that they can't evaluate yet.
 */

static inline node_t* __get_referred( int mindex, node_t *node ) {
	CFG_NODE_TO_SELF( node, mindex, char* );

	node_t *n = cfg_node_get( cur, ( char* ) ( self ) );
	
	// TODO: think about another errors
	if( ( n == NULL ) && ( cfg_error( NULL ) == CFG_NODE_DOES_NOT_EXIST ) ) {
		return NULL;
	}
}

#define PROXIFY( method ) \
static int _proxy_ ## method( int mindex, node_t *node, va_list args ) { \
	node_t *n = __get_referred( mindex, node ); \

	if( n == NULL ) { \
		return 0; \
	} \

	return _cfg_invoke_method( n, method, args ); \
}

/* Implementation of dumb proxy methods as it's needed for symlink */
PROXIFY( GET_NODE )
PROXIFY( GET_ITER )
PROXIFY( GET_NODE )
PROXIFY( ADD_NODE )
PROXIFY( CREATE_NODE )
PROXIFY( DEL_NODE )
PROXIFY( ACCEPT_ADVICE )
PROXIFY( GET_VALUE )
PROXIFY( SET_VALUE )

static methods_table_t _G_pointer_methods = {
	[ GET_ITER ] = _proxy_GET_ITER,
	[ GET_NODE ] = _proxy_GET_NODE,
	[ ADD_NODE ] = _proxy_ADD_NODE,
	[ CREATE_NODE ] = _proxy_CREATE_NODE,
	[ DEL_NODE ] = _proxy_DEL_NODE,
	[ ACCEPT_ADVICE ] = _proxy_ACCEPT_ADVICE,
	[ GET_VALUE ] = _proxy_GET_VALUE,
	[ SET_VALUE ] = _proxy_SET_VALUE
}

void* new( node_t *plugin_cfg ) {
	char *path = CFG_NODE_VALUE_TO( plugin_cfg, "cstr" );

	//TODO: handle case when value is absent
	//TODO: handle case when cast is failed

	mixin_t* m = cfg_mixin_alloc( _G_pointer_methods,
		sizeof( node_pointer_t ) + strlen( path ),
		0
	);

	CFG_MIXIN_TO_SELF( m, char* );

	strcpy( self, path );

	return self;
}

node_t* try_create( void* inst, node_t* parent ) {
	node_t *me = cfg_node_produce( parent );
	cfg_mixin_add( me, ( mixin_t* ) inst );
	
	return me;
}
