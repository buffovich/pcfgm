#include <pcfgm/types.h>

/*
 * TODO: separate what is needed and should be visible for lib user and what
 * is needed for iternal mechanics.
 */

/**
 * Get node with specified relative path.
 * Get descriptor of node with specified relative path according to
 * passed start node.
 * @param cfg config dscriptor
 * @param node start point for path walking
 * @param rel_path relative path to node from specified node;
 *                 if == NULL then source descriptor will be returned
 * @return descriptor binded to mentioned subtree or (spath == NULL)
 *         source; NULL if error occured
 * @see cfg_destroy()
 */
extern cfg_node_t cfg_node_lookup( cfg_node_t cur,
    const char* path
);

extern cfg_node_t cfg_node_link( cfg_node_t cur,
	cfg_node_t what,
    const char* name
);

extern cfg_node_t cfg_node_create( cfg_node_t parent );

extern int cfg_node_add_class( cfg_node_t n,
	class_t *klass,
	void *idata
);

extern cfg_node_t cfg_node_del( cfg_node_t cur,
    const char* name
);

/* Two methods for playing with reference count */

extern int cfg_node_mark( cfg_node_t what );

extern int cfg_node_unmark( cfg_node_t what );

extern int cfg_node_gc( cfg_node_t what );

/*
 * TODO: I know. It's ugly name. But what would you suggest? And it's
 * convenient and fast way to alert underlying plugin about particular
 * number of nodes we're going to add just after that.
 */
extern cfg_node_t cfg_node_advise_capacity( cfg_node_t node,
	unsigned int will_have_in_addition
);

/**
 * Get parent node of passed node.
 * @param node which parent we want to get
 * @return parent node of passed node or NULL if error occured
 * @see cfg_node_get()
 * @see cfg_node_get_path()
 * @see cfg_node_get_name()
 */
extern cfg_node_t cfg_node_get_parent( cfg_node_t node );

/**
 * Creates iterator.
 * Everything is simple - we create iterator for iterating through
 * children of passed subnode.
 * @param parent subnode path
 * @return iterator if everything is fine; NULL - otherwise; iterator
 *         should be utilized via cfg_destroy routine
 * @see cfg_iterator_t
 * @see cfg_iterator_next()
 */
extern cfg_iter_t cfg_node_iterator( cfg_node_t parent );

/**
 * Get next key-node pair.
 * Get key-node pair of the next child according to passed iterator's
 * position. We can get only key or only node descriptor. It depends on
 * what we pass as argument for key and node parameters.
 * @param iter iterator
 * @param key pointer to variable where pointer to key name will be
 *            stored; if NULL then key name won't be returned
 * @param node pointer to variable where node descriptor will be
 *             stored; if NULL then node won't be returned
 * @return >0 - ok; 0 - otherwise
 * @see cfg_iterator_get()
 * @see cfg_iterator_t
 */
extern cfg_node_t * cfg_iterate( cfg_iter_t iter, const char** key );

/**
 * Close config session/destroy any desriptor with associated buffers.
 * @param cfg config tree descriptor
 * @return >0 if everything OK, 0 otherwise
 * @see cfg_init()
 * @see cfg_init_as()
 */
extern int cfg_destroy( cfg_node_t cfg );
