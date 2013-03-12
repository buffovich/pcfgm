#include <pcfgm/types.h>

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
extern cfg_node_t cfg_node_succ( cfg_node_t cur,
    const char* forw_path
);

/**
 * backw_path should be treated as: name_in_parent/name_of_parent_at_gparent
 */
extern cfg_node_t cfg_node_pred( cfg_node_t cur,
    const char* backw_path
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
 * Represents iterator.
 * This thing is used in within operations related to iteration through
 * node children. As it has been mentioned before, we have tree-like
 * config structure. Some nodes may have variadic number of subnodes.
 * It may be used to construct hashtables in config. So we have ability
 * to iterate through keys and its values.
 * @see cfg_iterator_get()
 * @see cfg_iterator_next()
 */
typedef void* cfg_iter_t;

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
extern cfg_iter_t cfg_iterator_pred( cfg_node_t parent );

extern cfg_iter_t cfg_iterator_succ( cfg_node_t parent );

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
