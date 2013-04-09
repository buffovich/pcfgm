#ifndef API_PLUGIN
#define API_PLUGIN

/*
 * Constructor for plugin instance.
 * The only parameter is the configuration value packed to the blob.
 * Plugin can use node graph attched to the point plugin applied but
 * this solution relates to using methods described below.
 * Framework asking plugin create the node to continue tree building.
 * If NULL then plugin instance will be enqueued for the next round of evaluation.
 * The next phase hooks won't be called until this one has returned Node
 * instance.
 */
extern void* on_create( blob_t* icfg, node_t* parent );

/*
 * Framework is going to fill subtree attached to the place where plugin
 * has been instantiated. If FALSE is returned then plugin instance will be
 * enqueued for the next round of evaluation. The next phase hooks won't be
 * called until this one has returned TRUE instance.
 */
extern void* on_enter( void* with );

/*
 * Framework is going to leave filled subtree. At this stage subtree is
 * filled entirely. All subtree nodes should return successfully from their's
 * on_leave stages.
 */
extern void* on_leaving( void* with );

#endif
