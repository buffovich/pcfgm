#include <api/plugin.h>
#include <stdarg.h>
#include <dlfcn.h>

typedef struct {
	// TODO: do not forget parent as used (increase ref_count)
	// this is node suggested being the one which will be added to "parent"
	// with appropriate "name"
	node_t *node;
	// plugins which use the same resulting node and the same config
	// (intermixed)
	// and plugins which use the result of each other as config
	// each intermixed group is splitted from another one with NULL pointer
	plugin_constructor_t *queue;
	// current configuration root for the plugins
	// TODO: do not forget to mark this one as used
	node_t *cfg;

	// supposed name which "node" will be added to parent after final
	// evaluation
	char *name;
	// future parent (or one of, I mean it doesn't have to be
	// the one which is in "parent" field of node struct) for the "node"
	node_t *parent;
	// next postponed evaluation or NULL if it's the last one
	postponed_t *next;
} postponed_t;

typedef struct {
	postponed_t *queue;
	const char *plugin_fs_dir;
} plugger_t;

/*
 * Crucial part of the plugger. Loads modules and returns their constructors.
 */
static plugin_constructor_t __load_plugin( const char *path,
	const char *name
) {
	size_t plen = strlen( path );
	size_t nlen = strlen( name );
	char *fname = alloca( plen + 1 + nlen + 4 );
	char *cur = fname;
	
	strcpy( cur, name );
	// TODO: see above
	strcpy( cur += nlen, ".so" );
	
	// TODO: check opening and lookup for errors
	void *pin = dlopen( fname, RTLD_LAZY | RTLD_LOCAL | RTLD_NOLOAD );
	if( pin == NULL ) {
		cur = fname;
		strcpy( cur, path );
		// TODO: whoops! may experience troubles in Windows
		( cur += plen ) = '/';
		strcpy( cur += 1, name );
		// TODO: see above
		strcpy( cur += nlen, ".so" );
		pin = dlopen( fname, RTLD_LAZY | RTLD_LOCAL );
	}
		
	return dlsym( pin, "on_create" );
}

/*
 * Worker routine for the process of resolving unevaluated nodes. It is put
 * into per-thread working queue with associated pointer to "postponed" chain.
 * One run represents single evaluation try.
 */
static int _resolve_plugins( void *data ) {
	plugger_t *plugger = data;

	// If there is no more work - do not invoke me anymore
	if( plugger->queue == NULL )
		return 0;

	postponed_t *prev = plugger->queue, *curpp = plugger->queue;
	int isagain = 0;
	while( curpp != NULL ) {
		node_t *pnode;
		plugin_constructor_t *curc = curpp->queue;
		int isfailed = 0;
		while( *curc != NULL ) {
			for( ; *curc != NULL; ++curc ) {
				pnode = ( *curc )( curpp->cfg, curpp->node );

				if( pnode == NULL ) {
					// we are going to shift the tail of the list of
					// constructors
					isfailed = 1;
					goto END_EVAL;
				} else {
					if( pnode != curpp->node ) {
						// garbage collect created node if plugin return
						// another node (see plugin description; it might be
						// hardlink, for example)
						_cfg_node_unmark( curpp->node );
						_cfg_node_mark( curpp->node = pnode );
					}
				}
			}

			if( *( ++curc ) != NULL ) {
				_cfg_node_unmark( curpp->cfg );
				curpp->cfg = pnode;
				_cfg_node_mark(
					curpp->node = _cfg_node_create( curpp->parent )
				);
			}
		}

END_EVAL:
		if( isfailed ) {
			// shifting the rest of initialization queue
			// to the beginning
			__shift_constructors( curc, curpp->queue )
			// moving further in queue of postponed_t leaving just processed
			// one in place
			prev = curpp;
			curpp = curpp->next;
			isagain = 1;
		} else {
			_cfg_node_link( curpp->parent, curpp->node, curpp->name );
			// everuthing is OK and we should delete successful postponed
			// from the queue
			__remove_postponed( plugger, &prev, &curpp );
		}
	}

	return isagain;
}

static void __remove_postponed( plugger_t *plugger,
	postponed_t **prev,
	postponed_t **what,
) {
	postponed_t *next = ( *what )->next;
	
	if( *what == plugger->queue ) {
		// first element has its own rules
		// we should maintain the same pointer
		__destroy_postponed( *what );
		plugger->queue = *prev = *what = next;
		return;
	} else {
		// TODO: slab/tcalloc
		__destroy_postponed( *what );
		( *prev )->next = *what = next;
		return;
	}

	return;
}

static void __shift_constructors( plugin_constructor_t *from,
	plugin_constructor_t *to
) {
	if( from == to )
		return;

	for( plugin_constructor_t *fromc = from, *toc = to;
		( *( fromc - 1 ) != NULL ) && ( *( fromc - 2 ) != NULL )
		++fromc, ++toc
	)
		*toc = *fromc;
}

// add postponed evaluation description into the queue
// the tricky part is that "to" is a pointer to the first "postponed"
// structure regardless of the fact if there is at least one
// postponed element or queue is entirely empty. So
// if queue is empty, we should copy "what" directly into the "to".
// otherwise, we should insert "what" just after "to"
static void __add_postponed( postponed_t *what, plugger_t *to ) {
	// if first element is empty
	postponed_t *pnext = to->queue;
	to->queue = what;
	what->next = pnext;
}

// create posponed structure which describes the rest of "plugins"
// with given set of accompanying data (according to postponed_t)
static postponed_t *__create_postponed( const char *plugins,
	const char *plugin_fs_dir,
	node_t *node,
	node_t *cfg,
	plugin_constructor_t current,
	char *name,
	node_t *parent
) {
	// one for current and two for signalling NULL
	size_t num = 3;
	// count the real number of plugin's names. It's needed for queue
	// allocation
	for( const char *cur = plugins;
		*cur != '\0';
		cur = cur + strlen( cur ) + 1
	) {
		++num;
	}

	// TDO: slab/tcalloc
	plugin_constructor_t *cons = calloc( num, sizeof( plugin_constructor_t* ) );

	*cons = current;
	plugin_constructor_t *curc = cons + 1;
	for( char *cur = plugins;
		*cur != '\0';
		cur = cur + strlen( cur ) + 1, ++curc
	) {
		// if plugin's name == ">" then we should keep appropriate slot in
		// queue NULLified. It's the marker that resulting node from previous
		// "construction" should be used as configuration tree for the next
		// group of plugins
		if( *cur != '>' )
			*curc = __load_plugin( plugin_fs_dir, cur );
	}

	// to be sure, that chair will be in place when we will decide to
	// resolve postponed
	_cfg_node_mark( parent );
	_cfg_node_mark( cfg );
	_cfg_node_mark( node );

	// TODO: slab/tcalloc
	postponed_t *p = calloc( 1, sizeof( postponed_t ) );

	p->node = node;
	p->cfg = cfg;
	p->parent = parent;
	p->queue = cons;
	// TODO: tcalloc
	p->name = calloc( strlen( name ) + 1, sizeof( char ) );
	strcpy( p->name, name );

	return p;
}

static void __destroy_postponed( postponed_t *p ) {
	// TODO: slab/tcalloc
	cfg_node_unmark( p->cfg );
	cfg_node_unmark( p->node );
	cfg_node_unmark( p->parent );
	free( p->name );
	free( p->cons );
	free( p );
}

static int _link_node( node_t *node,
	class_t **klass,
	void **datapp,
	va_list args
) {
	const char *name = va_arg( args, ( const char* ) );
	
	if( name[ 0 ] == '#' ) {
		// OK. It's the special node which describes plugins insertion.
		node_t *cfg = va_arg( args, ( node_t* ) );
		// It looks like: #name => plugin_name\0[plugin_name\0...][">"]\0
		char *plugins = CFG_NODE_VALUE_TO( cfg, "cstr" );
		// TODO: check for emptiness and cast error
		node_t *newn = _cfg_node_create( node );
		node_t *pnode = newn;
		plugin_constructor_t pconstr;
		const char *plugin_fs_dir = ( ( plugger_t* ) *datapp )->plugin_fs_dir;
		for( char *cur = plugins;
			*cur != '\0';
			cur = cur + strlen( cur ) + 1
		) {
			if( *cur != '>' ) {
				// "intermix case"; we should mix the plugins
				pconstr = __load_plugin( plugin_fs_dir, cur );
				//TODO: check for loading errors

				// try to invoke constructor
				pnode = pconstr( cfg, newn );
				if( pnode == NULL ) {
					// is there something in queue?
					// if no then we should add the resolver for deffered
					// execution
					if( ( ( plugger_t* ) *datapp )->queue == NULL )
						_cfg_work_add( _resolve_plugins, datapp );
					// construction (or "mixing") failed - postpone it
					__add_postponed(
						__create_postponed( cur + strlen( cur ) + 1,
							newn, cfg,
							pconstr, name + 1,
							node
						),
						*datapp
					);
					return 0;
				} else {
					if( pnode != newn ) {
						// garbage collect created node if plugin return
						// another node (see plugin description; it might be
						// hardlink, for example)
						_cfg_node_gc( newn );
						newn = pnode;
					}
				}
			} else {
				// resulting tree should serve as configuration tree for the
				// next bunch of plugins; clean up previous configuration tree
				_cfg_node_gc( cfg );
				cfg = pnode;
				pnode = newn = _cfg_node_create( node );
			}
		}

		// plugins was inserted successfully - continue adding
		_cfg_method_superv( node, klass, datapp, LINK_NODE,
			name + 1, pnode
		);

		// No need to invoke anything - we have done it above
		return 0;
	}

	// Do your work, Framework. I'm not interested in this node.
	return 1;
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

static class_t _G_plugger = {
	.options = 0,
	.vtable = {
		[ LINK_NODE ] = _link_node,
		[ CREATE_NODE ] = _create_node
	}
};

node_t* on_create( node_t* cfg, node_t* me ) {
	// TODO: slab/tcalloc
	plugger_t *self = calloc( 1, sizeof( plugger_t ) );
	
	const char *fpath = CFG_VALUE_BY_PATH( cfg, "plugin_fs_dir", "cstr" );
	// TODO: handle all kinds of error above
	self->plugin_fs_dir = calloc( strlen( fpath ) + 1 , sizeof( char ) );
	strcpy( self->plugin_fs_dir, fpath );
	
	_cfg_node_add_class( me,
		&_G_plugger,
		self
	);

	return me;
}
