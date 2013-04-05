#include <api/plugin.h>

void* new( node_t *plugin_cfg ) {
	return CFG_NODE_VALUE_TO( plugin_cfg, "cstr" );
	//TODO: handle case when value is absent
	//TODO: handle case when cast is failed
}

node_t* try_create( void* inst, node_t* parent ) {
	node_t *n = cfg_node_get( cur, ( char* ) ( inst ) );
	
	// TODO: think about another errors
	if( ( n == NULL ) && ( cfg_error( NULL ) == CFG_NODE_DOES_NOT_EXIST ) ) {
		return NULL;
	}

	return n;
}
