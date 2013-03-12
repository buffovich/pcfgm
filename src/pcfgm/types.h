/**
 * Descriptor of node.
 * Whenever we open particular instance of config or pick particular
 * sub-tree/destination node by abolute or relative path, a value of this type
 * is returned. It's a handle for particular point in config tree/graph.
 * After using, handle should be destroyed.
 * @see cfg_state_t
 * @see cfg_init();
 * @see cfg_init_as();
 * @see cfg_destroy()
 */
typedef void* cfg_node_t;
