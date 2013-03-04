/**
 * Common definition of descriptor.
 * Descriptor is a quite common thing in the library. It is used for
 * config session binding, config node binding and so on.
 * @see cfg_t
 * @see cfg_node_t
 */
typedef void* cfg_descriptor_t;
/**
 * Descriptor of config.
 * When we open particular instance of config, this value is returned.
 * It reflects session of config using. We can open several instances
 * of the same config if it's needed. All changes made in one instance
 * will be propagated to all instances. Hooks will be invoked, change
 * descriptors will be changed. After using,
 * descriptor should be closed.
 * @see cfg_state_t
 * @see cfg_init();
 * @see cfg_init_as();
 * @see cfg_destroy()
 */
typedef cfg_descriptor_t cfg_t;
/**
 * Opens new session for default config.
 * Opens new session for config for current application name (argv[0]).
 * @return config session descriptor, NULL if error
 * @see cfg_init_as()
 * @see cfg_destroy()
 */
extern cfg_t cfg_init( void );
/**
 * Opens new session for config.
 * Opens new session for config for specified app name.
 * @param app_id  application name (or ID), default if NULL
 * @return config session descriptor, NULL if error
 * @see cfg_init()
 * @see cfg_destroy()
 */
extern cfg_t cfg_init_as( const char* app_id );
/**
 * Close config session/destroy any desriptor with associated buffers.
 * @param cfg config session descriptor
 * @return >0 if everything OK, 0 otherwise
 * @see cfg_init()
 * @see cfg_init_as()
 */
extern int cfg_destroy( cfg_descriptor_t cfg );
/**
 * Splits key's path.
 * This structure accelerates access to elements of path and it's
 * more convinient for library code. But there is just a little extra
 * space is used because "components" point to different parts of
 * the same string which was passed to cfg_split_path. So, it means that
 * you should handle string copy yourself.
 * @see cfg_path_split()
 */
typedef struct _cfg_key_path {
    int num; /**< Number of components */
    const char* components[]; /**< Components themselves. There aren't
        NULL-terminated strings because these are just pointers to
        different parts of source string. So, key name will end up on
        '.' or \0 */
} cfg_key_path_t;
/**
 * Splitting routine for path.
 * Pass path and allocated structure for splitted path to it and get
 * splitted path linked to source string.
 * @param path source string which will be splitted
 * @return newly created 'splitting'; should be cleaned later by caller
 *         through standard free(); NULL - if error
 */
extern cfg_key_path_t* cfg_path_split( const char* path );
/**
 * Node descriptor.
 * This entity is used to play with nodes within config.
 * @see cfg_node_get()
 * @see cfg_node_get_path()
 * @see cfg_node_get_name()
 * @see cfg_node_get_parent()
 * @see cfg_node_lock()
 * @see cfg_node_unlock()
 * @see cfg_node_watch()
 * @see cfg_node_unwatch()
 * @see cfg_node_get_watcher()
 * @see cfg_data_get()
 * @see cfg_data_set()
 * @see cfg_iterator_get()
 * @see cfg_iterator_next()
 */
typedef cfg_descriptor_t cfg_node_t;
/**
 * Get root node of config.
 * @param cfg config descriptor
 * @return root node of config
 * @see cfg_node_get()
 */
extern cfg_node_t cfg_get_root( cfg_t cfg );

