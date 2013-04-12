#include <api/plugin.h>
#include <stdarg.h>
#include <dlfcn.h>

typedef struct {
	// TODO: do not forget parent as used (increase ref_count)
	node_t *node;
	plugin_constructor_t *constructor;
	// TODO: do not forget to mark this one as used
	node_t *cfg;
	postponed_t *next;
} postponed_t;

static int _resolve_plugins( void *data ) {
	postponed_t *queue = data;
	
}

static void __add_postponed( node_t *node,
	plugin_constructor_t *constructor,
	node_t *cfg,
	postponed_t *to
) {
	// if first element is empty
	postponed_t *p;
	
	if( to.name == NULL ) {
		p = to;
		_cfg_work_add( _resolve_plugins, to );
	} else {
		p = calloc( 1, sizeof( postponed_t ) );
		p->next = to->next;
		to->next = p;
	}

	cfg_node_mark( p->node = node );
	p->constructor = constructor;
	cfg_node_mark( p->cfg = cfg );
	p->name = name;
}

static plugin_constructor_t __load_plugin( const char *name ) {
	
}

static int _link_node( node_t *node,
	class_t **klass,
	void **datapp,
	va_list args
) {
	const char *name = va_arg( args, ( const char* ) );
	
	if( name[ 0 ] == '#' ) {
		node_t *cfg = va_arg( args, ( node_t* ) );
		char *plugins = CFG_NODE_VALUE_TO( cfg, "cstr" );
		// TODO: check for emptiness and cast error
		node_t *newn = cfg_node_create( node );
		node_t *pnode = newn;
		plugin_constructor_t pconstr;
		for( char *cur = plugins;
			*cur != '\0';
			cur = cur + strlen( cur ) + 1
		) {
			pconstr = __load_plugin( cur );
			//TODO: check for loading errors

			pnode = pconstr( cfg, pnode );
			if( pnode == NULL ) {
				__add_postponed( pnode, pconstr, cfg, cur, *datapp );
			} else {
				if( pnode != newn ) {
					_cfg_node_unmark( newn );
					newn = pnode;
				}
			}
		}
		
		return 0;
	}

	return 1;
	* Third parameter - const char *name
	 * Forth parameter - node_t *what
}

static int _create_node( node_t *node,
	class_t **klass,
	void **datapp,
	va_list args
) {
	_cfg_method_super( node, klass, datapp, CREATE_NODE, args );

	node_t *node = *( va_arg( args, ( node_t ** ) ) );
	_cfg_ptr_add( *datapp, node );

	return 0;
}

/*
 * What do we need?
 * We need set_value - react appropriatelly if magic combination arrives:
 *   1) add node with "#node_name:plugin_name" name - preparing for plugin
 *      instance creation;
 *   2) set the value for this node - invoking before_create with corresponding
 *      blob
 */

static class_t _G_plugger = {
	.options = 0,
	.vtable = {
		[ LINK_NODE ] = _link_node,
		[ CREATE_NODE ] = _create_node
	}
};

node_t* on_create( node_t* cfg, node_t* me ) {
	_cfg_node_add_class( me,
		&_G_plugger,
		calloc( 1, sizeof( postponed_t ) )
	);

	return me;
}
