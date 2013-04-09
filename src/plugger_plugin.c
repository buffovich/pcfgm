#include <api/plugin.h>

/*
 * What do we need?
 * We need set_value - react appropriatelly if magic combination arrives:
 *   1) add node with "#node_name:plugin_name" name - preparing for plugin
 *      instance creation;
 *   2) set the value for this node - invoking before_create with corresponding
 *      blob
 */

static methods_table_t _G_pointer_methods = {
	[ GET_ITER ] = _proxy_GET_ITER,
	[ GET_NODE ] = _proxy_GET_NODE,
	[ ADD_NODE ] = _proxy_ADD_NODE,
	[ CREATE_NODE ] = _proxy_CREATE_NODE,
	[ DEL_NODE ] = _proxy_DEL_NODE,
	[ ACCEPT_ADVICE ] = _proxy_ACCEPT_ADVICE,
	[ GET_VALUE ] = _proxy_GET_VALUE,
	[ SET_VALUE ] = _proxy_SET_VALUE
};

void* before_create( blob_t* icfg ) {
}

node_t* on_create( void* with, node_t* parent ) {
}

int on_enter( node_t *to ) {
}

int on_leaving( node_t *from ) {
}
