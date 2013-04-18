#ifndef PCFGM_GRAPH
#define PCFGM_GRAPH

#include <pcfgm/types.h>

/**
 * Opens new session for default config.
 * Opens new session for config for current application name (argv[0]).
 * @return config tree descriptor, NULL if error
 * @see cfg_init_as()
 * @see cfg_destroy()
 */
extern node_t *cfg_init( void );
/**
 * Opens new session for config.
 * Opens new session for config for specified app name.
 * @param app_path  application name (or ID), default if NULL
 * @return config tree descriptor, NULL if error
 * @see cfg_init()
 * @see cfg_destroy()
 */
extern node_t *cfg_init_as( const char* app_path );
/**
 * Lookup node with specified relative/absolute path.
 * Lookup node with specified relative/absolute path according to
 * passed start node.
 * @param cur start point for path walking
 * @param path relative path to node from specified node;
 *             if == NULL then cur will be returned
 * @return node binded to mentioned subtree or (spath == NULL)
 *         source; NULL if error occured
 * @see cfg_destroy()
 * @see cfg_node_link()
 */
extern node_t *cfg_node_lookup( node_t *cur, const char* path );
/**
 * Link node to specified relative/absolute path.
 * Link node to specified relative/absolute path according to
 * passed start node.
 * @param cur start point for path walking
 * @param what node will be binded to mentioned subtree; NULL if error occured
 * @param path relative path to node from specified node;
 *             if == NULL then cur will be returned
 * @see cfg_destroy()
 * @see cfg_node_link()
 */
extern int cfg_node_link( node_t *cur, node_t *what, const char* name );

extern node_t *cfg_node_create( node_t *parent );

extern node_t *cfg_node_unlink( node_t *cur, const char* name );

/**
 * Get parent node of passed node.
 * @param node which parent we want to get
 * @return parent node of passed node or NULL if error occured
 * @see cfg_node_get()
 * @see cfg_node_get_path()
 * @see cfg_node_get_name()
 */
extern node_t *cfg_node_get_parent( node_t *node );

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
extern iter_t *cfg_iterator_get( node_t *parent );

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
extern node_t *cfg_iterator_next( iter_t *iter, const char** key );

/**
 * Close config session/destroy any desriptor with associated buffers.
 * @param cfg config tree descriptor
 * @return >0 if everything OK, 0 otherwise
 * @see cfg_init()
 * @see cfg_init_as()
 */
extern int cfg_node_destroy( node_t *node );

#endif PCFGM_GRAPH
