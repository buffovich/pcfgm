#include <api/plugin.h>

void* before_create( blob_t* icfg ) {
	return CFG_VALUE_TO( icfg, "cstr" );
	//TODO: handle case when value is absent
	//TODO: handle case when cast is failed
}

node_t* on_create( void* with, node_t* parent ) {
	node_t *n = cfg_node_get( parent, ( char* ) ( with ) );
	
	// TODO: think about another errors
	if( ( n == NULL ) && ( cfg_error( NULL ) == CFG_NODE_DOES_NOT_EXIST ) ) {
		return NULL;
	}

	return n;
}

int on_enter( node_t *to ) {
	return 1;
}

int on_leaving( node_t *from ) {
	return 1;
}
