#ifndef API_PLUGIN
#define API_PLUGIN

/*
 * Constructor for plugin instance.
 * It takes configuration of instance as node graph.
 * Yes, plugin config is graph too. Mathematically clear.
 * Returns instance for passing it to try_bind.
 */
extern void* new( node_t* cfg );

/*
 * The idea is to stick particular plugin instance to the Node object.
 * Method performs mixing if needed (in the most cases, actually).
 * It can return the result of "binding". If its result is NULL then
 * appropriate plugin instance will be enqueued into "initialization-pending"
 * list and will be invoked again at the next pass of config evaluation with
 * the same instance parameter. This feature is actual for hardlinks,
 * for example. Because of inevitable influence of execution sequence. And
 * hardlink can refer at particular evaluation iteration to the destination
 * at the graph which doesn't exist yet (for example external configuration
 * tree).
 * TODO: Framework should pass start node here. Node should be allocated already
 */
extern node_t* try_create( void* inst, node_t* parent );

#endif
