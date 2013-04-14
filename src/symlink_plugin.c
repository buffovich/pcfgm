#include <api/plugin.h>
#include <stdarg.h>

/* be careful ! Soft links will work much slower then hard ones.*/
/* now, think about situation when we are referencing to the branch
 * which is not accessible yet but it will at the end of pass 1.
 * Probably, we need pass #2?
 *
 * Here is the idea:
 * we enqueue nodes/plugins which report that they can't evaluate yet.
 */

#define PROXIFY( method ) \
static int _proxy_ ## method( node_t *node, \
	class_t **klass, \
	void **datapp, \
	va_list args \
) { \
	node_t *n = _cfg_node_lookup( cur, ( char* ) ( *datapp ) ); \

	if( n == NULL ) { \
		return 0; \
	} \

	_cfg_method_invoke( n, method, args ); \
	
	return 1; \
}

// TODO: opaque moment; for now let's ask framework to invoke next methos
// in the sequence

/* Implementation of dumb proxy methods as it's needed for symlink */
PROXIFY( GET_NODE )
PROXIFY( GET_ITER )
PROXIFY( LOOKUP_NODE )
PROXIFY( LINK_NODE )
PROXIFY( CREATE_NODE )
PROXIFY( DEL_NODE )
PROXIFY( ACCEPT_ADVICE )
PROXIFY( GET_VALUE )
PROXIFY( SET_VALUE )

static int _destroy_symlink( node_t *node,
	class_t **klass,
	void **datapp,
	va_list args
) {
	// TODO: think about alternative allocating mechanisms
	free( *datapp );

	return 1;
}

static class_t _G_symlink = {
	.options = CLASS_UNINHERITABLE,
	.vtable = {
		[ GET_ITER ] = _proxy_GET_ITER,
		[ LOOKUP_NODE_NODE ] = _proxy_LOOKUP_NODE,
		[ LINK_NODE ] = _proxy_ADD_NODE,
		[ CREATE_NODE ] = _proxy_CREATE_NODE,
		[ DEL_NODE ] = _proxy_DEL_NODE,
		[ ACCEPT_ADVICE ] = _proxy_ACCEPT_ADVICE,
		[ GET_VALUE ] = _proxy_GET_VALUE,
		[ SET_VALUE ] = _proxy_SET_VALUE,
		[ DESTROY ] = _destroy_link
	}
}

node_t* on_create( node_t* cfg, node_t* me ) {
	char *path = CFG_VALUE_BY_PATH( cfg, "to", "cstr" );

	if( path == NULL )
		return NULL;
	//TODO: handle case when node is absent
	//TODO: handle case when value is absent
	//TODO: handle case when cast is failed

	char* self = calloc( strlen( path ) + 1, 1 );

	strcpy( self, path );

	_cfg_node_add_class( me, &_G_symlink, self );
	
	return me;
}
