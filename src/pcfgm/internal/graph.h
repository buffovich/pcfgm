#ifndef PCFGM_INTERNAL_GRAPH
#define PCFGM_INTERNAL_GRAPH

#include <pcfgm/types.h>

extern void _cfg_node_initialize( node_t *n );

extern void _cfg_node_finalize( node_t *n );

extern void _cfg_node_add_class( node_t *n, class_t *klass, void *idata );

extern void _cfg_node_make_child( node_t *parent, node_t *child );

/* Two methods for playing with reference count */

extern int _cfg_node_mark( node_t *what );

extern int _cfg_node_unmark( node_t *what );

extern int _cfg_node_gc( node_t *what );

/*
 * TODO: I know. It's ugly name. But what would you suggest? And it's
 * convenient and fast way to alert underlying plugin about particular
 * number of nodes we're going to add just after that.
 */
extern cfg_node_t _cfg_node_advise_capacity( node_t *node,
	unsigned int will_have_in_addition
);

extern int _cfg_method_superv( node_t *node,
	class_t **klass,
	void **datapp,
	method_id_t m,
	va_list args
);

static inline int _cfg_method_super( node_t *node,
	class_t **klass,
	void **datapp,
	method_id_t m,
	...
) {
	va_list args;
	va_start( args, m );
	int ret = _cfg_method_superv( node, klass, datapp, m, args );
	va_end( args );
	return ret;
}

static inline int _cfg_method_invokev( node_t *node,
	method_id_t m,
	va_list args
) {
	return _cfg_method_superv( node, node->class, node->head, m, args );
}

static inline int _cfg_method_invoke( node_t *node,
	method_id_t m,
	...
) {
	va_list args;
	va_start( args, m );
	int ret = _cfg_method_superv( node, node->class, node->head, m, args );
	va_end( args );
	return ret;
}

#endif PCFGM_INTERNAL_GRAPH
