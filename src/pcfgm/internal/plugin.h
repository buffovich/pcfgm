#ifndef PCFGM_INTERNAL_PLUGIN
#define PCFGM_INTERNAL_PLUGIN

#include <pcfgm/types.h>

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
extern node_t* on_create( node_t *cfg, node_t *me );

/*
 * The idea of "me" parameter is to provide plugin with created node; if
 * created node doesn't fit plugin then plugin contructor may safely
 * return pointer to another node. It shouldn't deallocate "me". It will be
 * done by framework.
 */
typedef node_t* ( *plugin_constructor_t )( node_t* cfg, node_t* me );

// make self pointer from blob double pointer
#define CFG_PPTR_TO_SELF( pp, mtype ) ( mtype ) *self = \
	( ( mtype )* ) ( *( pp ) );

#endif
